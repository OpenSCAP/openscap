#!/usr/bin/env bash
#############################################################################################

VG=$(which valgrind)
VGOPT="--trace-children=yes --leak-check=full --show-reachable=yes --leak-resolution=high --suppressions=${srcdir}/openscap.supp"
VERBOSE=0
DEFAULT_REGEX='^.*(oscap-scan|test_probes|probe_|oscap).*$'

#############################################################################################
function imsg {
    if (( $VERBOSE == 1 )); then
        echo -n '[i] '
        echo $*
    fi
}

function wmsg {
    echo -n '[w] '
    echo $*
}

function emsg {
    echo -n '[e] '
    echo $*
}

function cpy {
    local src=$1
    local dst=$2

    imsg "$src ==> $dst"
    cp   "$src"   "$dst"
}
#############################################################################################

prog="$(basename "$0")"

while getopts "r:vh" opt; do
    case "$opt" in
        r)
            REGEX="$OPTARG"
            ;;
        v)
            VERBOSE=1
            ;;
        h)
            echo "Usage: $prog [-hr] <command>"
            echo "       -r    Filename regex (extended)"
            echo "       -v    Be verbose"
            echo "       -h    This help"
            echo ""
            exit 0
            ;;
        *)
            emsg "wtf? $opt"
            exit 1
            ;;
    esac
done
shift $((OPTIND - 1))

COMM="$1"

if [[ -z "$VG" ]]; then
    emsg "Please intall valgrind first or correct your \$PATH."
    exit 1
else
    VER="$($VG --version)"
    case "$VER" in
        valgrind-3.[3456].*)
            ;;
        *)
            wmsg "The version of Valgrind ($VG) you are using wasn't tested with this script. Don't trust the output of this script."
            ;;
    esac
fi

if [[ -z "$COMM" ]]; then
    emsg "Executing rm -rf / ... this may take a while"
    exit 2
fi

if [[ -z "$REGEX" ]]; then
    REGEX="$DEFAULT_REGEX"
    imsg "Using default filename regex: $REGEX"
fi

TMPDIR=$(mktemp -d -t vgrun.XXXX)

imsg " outdir: $TMPDIR"
imsg "command: $VG $VGOPT --log-file=\"$TMPDIR/output.%p\" -- $COMM"

PREFIX="$$-$(date +%H%M)"
SUFFIX=".vglog"

echo "=== ID: $PREFIX === CMD: $COMM ==="

$VG $VGOPT --log-file="$TMPDIR/output.%p" -- $COMM > /dev/null 2>&1

if (( $? == 134 )); then
    exit 134
fi

c=0
LOG=()

for log in "$TMPDIR"/output.*; do
    log_cmd0="$(sed -n 's|^.*Command:[[:space:]]*\(.*\)[[:space:]]*$|\1|p' "$log")"

    if [[ -z "$log_cmd0" ]]; then
    	# Different output format
    	log_cmd0="$(grep -A 1 '^.*My PID = [0-9]*, parent PID = [0-9]*\.[[:space:]]*Prog and args are:.*$' "$log")"


    	if [[ -z "$log_cmd0" ]]; then
    	    emsg "Don't know how to parse valgrind output :["
    		# TODO: xml output parsing?
    		exit 3
    	fi

    	log_cmd0="$(echo "$log_cmd0" | tail -n 1 | sed -n 's|^==[0-9]*==[[:space:]]*\(.*\)$|\1|p')"
    fi

    if file "$(echo -n "$log_cmd0" | sed -n 's|^\([^[:space:]]*\).*$|\1|p')" | grep -q 'ELF.*executable'; then
	    log_cmd1="$(basename "$(echo "$log_cmd0" | sed -n 's|^\([^[:space:]]*\).*$|\1|p')")"

	    if echo "$log_cmd1" | egrep -q "$REGEX"; then
    	   	 outfile="$PREFIX-$c.$log_cmd1$SUFFIX"
       		 cpy "$log" "$outfile"
       	 	 LOG[$c]="$outfile"
        	 CMD[$c]="$log_cmd1"
        	 c=$(($c + 1))
    	    fi
    fi
done

rm -rf "$TMPDIR"

for ((i=0; i < ${#LOG[@]}; i++)); do
    num_le="$(egrep -ci "(lost in loss record|still reachable in loss record)" "${LOG[$i]}")"
    num_ir="$(grep -ci  "invalid read of size"                                 "${LOG[$i]}")"
    num_iw="$(grep -ci  "invalid write of size"                                "${LOG[$i]}")"
    num_ij="$(grep -ci  "conditional jump or move depends"                     "${LOG[$i]}")"
    num_iv="$(grep -ci  "use of uninitialized value"                           "${LOG[$i]}")"

    sum=$(($num_le + $num_ir + $num_iw + $num_ij + $num_iv))

    printf '[ %28s ] L=%3d, IR/W=%3d/%3d, IJ=%3d, UV=%3d .... E= %d\n' "${CMD[i]}" $num_le $num_ir $num_iw $num_ij $num_iv $sum

    if (( $sum == 0 )); then
        rm "${LOG[$i]}"
    fi
done
echo ""
