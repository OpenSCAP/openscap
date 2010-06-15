#!/usr/bin/env bash

VG=$(which valgrind)
VGOPT="--trace-children=yes --leak-check=full --show-reachable=yes"
COMM="$1"
REGEX="$2"

function cpy {
    local src=$1
    local dst=$2

#    echo "$src ==> $dst"
    cp "$src" "$dst"
}

if [[ -z "$VG" ]]; then
    echo "[e] Please intall valgrind first or correct your \$PATH."
    exit 1
fi

if [[ -z "$COMM" ]]; then
    echo "[e] Executing rm -rf / ... this may take a while"
    exit 2
fi

TMPDIR=$(mktemp -d)

#echo "[i]  outdir: $TMPDIR"
#echo "[i] command: $VG $VGOPT --log-file=\"$TMPDIR/output.%p\" -- $COMM"

PREFIX="$$-$(date +%H%M)"
echo "=== ID: $PREFIX === CMD: $COMM ==="

SEAP_DEBUGLOG_DISABLE=1 SEXP_VALIDATE_DISABLE=1 $VG $VGOPT --log-file="$TMPDIR/output.%p" -- $COMM > /dev/null 2>&1

c=0
LOG=()

for log in "$TMPDIR"/output.*; do
    log_cmd0="$(sed -n 's|^.*Command:[[:space:]]*\(.*\)[[:space:]]*$|\1|p' "$log")"

    if [[ -z "$log_cmd0" ]]; then
    	# Different output format
    	log_cmd0="$(grep -A 1 '^.*My PID = [0-9]*, parent PID = [0-9]*\.[[:space:]]*Prog and args are:.*$' "$log")"

    	if [[ -z "$log_cmd0" ]]; then
    		echo "[e] Don't know how to parse valgrind output :["
    		# TODO: xml output parsing?
    		exit 3
    	fi

    	log_cmd0="$(echo "$log_cmd0" | tail -n 1 | sed -n 's|^==[0-9]*==[[:space:]]*\(.*\)$|\1|p')"
    fi
    
    log_cmd1="$(basename "$(echo "$log_cmd0" | sed -n 's|^\([^[:space:]]*\).*$|\1|p')")"

    case "$log_cmd1" in
        *oscap-scan*)
            cpy "$log" $PREFIX-$c.oscap_scan
            LOG[$c]="$PREFIX-$c.oscap_scan"
            CMD[$c]="$log_cmd1"
            c=$(($c + 1))
            ;;
        *test_probes*)
            cpy "$log" $PREFIX-$c.test_probes
            LOG[$c]="$PREFIX-$c.test_probes"
            CMD[$c]="$log_cmd1"
            c=$(($c + 1))
            ;;
        *probe_*)
            cpy "$log" $PREFIX-$c.$log_cmd1
            LOG[$c]="$PREFIX-$c.$log_cmd1"
            CMD[$c]="$log_cmd1"
            c=$(($c + 1))
            ;;
    esac
done

for ((i=0; i < ${#LOG[@]}; i++)); do
    num_le="$(grep -c "lost in loss record"      "${LOG[$i]}")"
    num_ir="$(grep -c "invalid read of size"     "${LOG[$i]}")"
    num_iw="$(grep -c "invalid write of size"    "${LOG[$i]}")"
    num_ij="$(grep -c "conditional jump depends" "${LOG[$i]}")"

    sum=$(($num_le + $num_ir + $num_iw + $num_ij))

    printf '[ %28s ] leaks=%3d, inv.r/w=%3d/%3d, inv.jumps=%3d ...... E= %d\n' "${CMD[i]}" $num_le $num_ir $num_iw $num_ij $sum

    if (( $sum == 0 )); then
        rm "${LOG[$i]}"
    fi
done
echo ""
