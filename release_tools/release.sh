JENKINS_SITE='https://jenkins.open-scap.org'
GITHUB_ROOT='https://github.com/OpenSCAP/openscap'

test -f .env && . .env

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OSCAP_REPO_ROOT="$(cd "$SCRIPT_DIR" && cd ../.. && pwd)"
LIBRARY_STASH="/tmp/library-stash"
BUILDDIR="$OSCAP_REPO_ROOT"

FILE_WITH_VERSIONS="$SCRIPT_DIR/versions.sh"

. "$FILE_WITH_VERSIONS"


die()
{
    echo "$1"
    exit 1
}


clean_repository()
{
    make distclean
    git clean -f
}


clean_repository_aggressively()
{
    git reset --hard
    git clean -x -f -d
}


check_cpe()
{
    grep -q "cpe:/o:fedoraproject:fedora:$latest_fedora" "$OSCAP_REPO_ROOT/cpe/openscap-cpe-dict.xml" || die "Couldn't find Fedora $latest_fedora CPE"
    grep -q "cpe:/o:redhat:enterprise_linux:$latest_rhel" "$OSCAP_REPO_ROOT/cpe/openscap-cpe-dict.xml" || die "Couldn't find RHEL $latest_rhel CPE"
}


check_python_bindings()
{
    check_python_binding 2.7
    check_python_binding 3
}


# Args:
# $1: Python version
check_python_binding()
{
    local _python_major
    _python_major="${1%%.*}"
    (
        cd $OSCAP_REPO_ROOT/build \
            && LD_PRELOAD="/usr/lib64/libpython$1.so" ldd -r "swig/python$_python_major/.libs/_openscap_py.so" | grep -q '^undefined' \
            && die "Python $1 bindings seem to have undefined symbols"
    )
}


trigger_tests()
{
    xdg-open "$JENKINS_SITE/job/openscap-parametrized/build"
}


trigger_docbuild()
{
    xdg-open "$JENKINS_SITE/job/static_openscap_docs/build"
}


ensure_test_prerequisities()
{
    systemctl -q is-active sendmail || die "As of 11/2017, you have to have 'sendmail' running for tests to succeed."
}

build_all()
{
    (cd "$OSCAP_REPO_ROOT" && ./autogen.sh) || die "Error generating the configure script"
    (mkdir -p "$BUILDDIR" && cd "$BUILDDIR" && "$OSCAP_REPO_ROOT/configure" --enable-sce --enable-debug --enable-python3 && make) || die "Error building pristine OpenScap"
}


execute_local_tests()
{
    ensure_test_prerequisities
    build_all
    (cd "$BUILDDIR" && make check && make distcheck || die "Error during test run")
    check_python_bindings
}


make_dist()
{
    (
        cd "$BUILDDIR"
        make dist
        sha1sum openscap-$version.tar.gz > openscap-$version.tar.gz.sha1sum
        sha512sum openscap-$version.tar.gz > openscap-$version.tar.gz.sha512sum
    )
}


check_abi()
{
    rpm -q abi-compliance-checker || sudo dnf install abi-compliance-checker
    test -d openscap-abi-check || git clone https://github.com/OpenSCAP/openscap-abi-check
    (
        cd openscap-abi-check
        perl run_check.pl "$previous_version" "maint-$version_major_minor"
        echo 'Read the report, if there is an ABI issue (some symbols were changed / removed), fix it before proceeding further!'
        printf "%s\n" "Then, Change directory to '$(pwd)' and add the reports. Then, commit with the appropriate message:" "" "cd $(pwd)" "git add reports/${version}_maint-$version_major_minor.html'" "git commit -m 'Report before $2 release'"
    )
}


parse_variable_assignment_from_file()
{
    local _variable="$1" _fname="$2"
    test "$(grep "$_variable=[0-9]\+" "$_fname" | wc -l)" = 1 || die "More than one occurence of numerical assignment to '$_variable'"
    echo $(grep "$_variable=[0-9]\+" "$_fname" | sed -e "s/$_variable=\([0-9]\+\).*/\1/")
}


substitute_variable_assignment_value_in_file()
{
    local _variable="$1" _old_value="$2" _new_value="$3" _fname="$4"
    sed -i "s/$_variable=$_old_value/$_variable=$_new_value/" "$_fname"
}


get_lt_triplet_from_file()
{
    local _varname _fname="$1"
    for _varname in LT_CURRENT LT_REVISION LT_AGE
    do
        printf "%s " "$(parse_variable_assignment_from_file "$_varname" "$_fname")"
    done
    echo
}

#
# $1, $2, ... Files to probe
get_lt_triplet_from_files()
{
    local _first_file="$1" _from_first _from_next
    _from_first=$(get_lt_triplet_from_file "$_first_file")
    while [ $# -gt 1 ]
    do
        shift
        _from_next=$(get_lt_triplet_from_file "$1")
        test "$_from_first" == "$_from_next" || die "Inconsistent data from '$_first_file' and from '$1': Got '$_from_first' and '$_from_next' respectively."
    done
    echo "$_from_first"
}


increment_on_backwards_compatible()
{
    local triplet
    triplet=( $1 )
    (( triplet[0]++ ))
    (( triplet[2]++ ))
    printf "%s %s %s \n" "${triplet[@]}"
}


increment_on_bugfix()
{
    local triplet
    triplet=( $1 )
    (( triplet[1]++ ))
    printf "%s %s %s \n" "${triplet[@]}"
}


increment_on_breaking_change()
{
    local triplet
    triplet=( $1 )
    (( triplet[0]++ ))
    triplet[2]=0
    printf "%s %s %s \n" "${triplet[@]}"
}

# Args:
# $1: Triplet to be replaced
# $2: The replacement
# $3,$4, ...: Concerned files
apply_triplets_to_files()
{
    local _previous="$1" _replacement="$2" _fname
    shift 2
    for _fname in "$@"
    do
        apply_triplets_to_file "$_previous" "$_replacement" "$_fname"
    done
}

# Args:
# $1: Triplet to be replaced
# $2: The replacement
# $3: The concerned file
apply_triplets_to_file()
{
    local _previous=( $1 ) _replacement=( $2 ) _file="$3" _varnames=(LT_CURRENT LT_REVISION LT_AGE)
    for i in 0 1 2
    do
        substitute_variable_assignment_value_in_file "${_varnames[$i]}" "${_previous[$i]}" "${_replacement[$i]}" "$_file"
    done
}


#
# $1: Stragegy (backwards_compatible, bugfix, breaking_change)
increment_ltversions()
{
    local _files=("$OSCAP_REPO_ROOT/configure.ac" "$OSCAP_REPO_ROOT/ac_probes/configure.ac.tpl") _old_versions _new_versions _new_soname _old_soname
    # check_for_clean_repo
    _old_versions="$(get_lt_triplet_from_files "${_files[@]}")" || die "Unable to get current LT versions"
    _new_versions="$(increment_on_$1 "$_old_versions")" || die  "Unable to get calculate refreshed LT version with strategy '$1'"
    _old_soname="$(get_soname_from_triplet "$_old_versions")"
    _new_soname="$(get_soname_from_triplet "$_new_versions")"

    test "$_old_soname" = "$_new_soname" && die "The new and old sonames are the same '$_new_soname', which doesn't make sense."

    check_for_clean_repo
    clean_library_stash

    echo "Building with the old soname"
    build_all 2> /dev/null > /dev/null || die "Build with the old soname failed"
    stash_library
    check_stash_for_soname "$_old_soname" || die "Couldn't find the expected (old) soname $_old_soname, did it compile?"
    check_stash_for_soname "$_new_soname" && die "Unexpectedly found new soname $_new_soname where only the old soname $_old_soname is supposed to be."

    apply_triplets_to_files "$_old_versions" "$_new_versions" "${_files[@]}"

    echo "Building with the new soname"
    build_all 2> /dev/null > /dev/null || die "Build with the new soname failed"
    stash_library
    check_stash_for_soname "$_new_soname" || die "Couldn't find the expected (old) soname $_old_soname, did it compile?"
    echo "Build went as expected, showing git diff."

    git diff
    printf "%s\n" "If you are satisfied with the diff, you can commit. Execute " "git add ${_files[*]}" "" "Then 'git commit' with message" "" "Bump soname from $_old_soname to $_new_soname" "" "<X> new symbols were added, <Y> were removed."
}


# Args:
# $1: The string in form of "LT_CURRENT LT_REVISION LT_AGE"
get_soname_from_triplet()
{
    local _triplet=( $1 )
    printf "%s.%s.%s" $((_triplet[0] - _triplet[2])) ${_triplet[2]} ${_triplet[1]}
}


# Args:
# $1: Filename that contains possible duplications.
resolve_name_duplication()
{
    . "$SCRIPT_DIR/naming.sh"
    local _tempfile=/tmp/names-dedup
    cp "$1" "$_tempfile"
    for bad_name in "${!name_mapping[@]}"
    do
        sed -i "s/$bad_name/${name_mapping[$bad_name]}/" "$_tempfile"
    done
    cat "$_tempfile" | sort | uniq > "$1"
    rm -- "$_tempfile"
}


get_new_authors()
{
    local _all_time_authors="/tmp/all_time_authors" _recent_authors="$1" _new_authors="$2"
    git log | grep '^Author' | cut -f 1 --complement -d ' ' | sort | uniq > "$_all_time_authors"
    resolve_name_duplication "$_all_time_authors"
    git log "$previous_version".. | grep '^Author' | sort | cut -f 1 --complement -d ' ' | uniq > "$_recent_authors"
    resolve_name_duplication "$_recent_authors"
    diff -U 0 all_time_authors recent_authors | grep -e '^+Author' | sort > "$_new_authors"
    rm "$_all_time_authors"
}


clean_library_stash()
{
    rm -rf "$LIBRARY_STASH"
}


stash_library()
{
    mkdir -p "$LIBRARY_STASH"
    cp "$BUILDDIR/src/.libs/"libopenscap.so.* "$LIBRARY_STASH/"
}

# Args:
# $1: The soname to check for (e.g. 1.2.3)
check_stash_for_soname()
{
    local _regex
    _regex="\.so\.$(echo "$1" | sed -e 's/\./\\./g')$"
    ls "$LIBRARY_STASH" | grep -q "$_regex"
}


check_for_clean_repo()
{
    # check that there is nothing to report by 'status' except untracked files.
    git status --porcelain=v2 | grep -q --invert-match '^?' && die "The repository is not clean, stash your changes to proceed."
}


check_that_bump_is_appropriate()
{
    check_for_clean_repo
    # check that there is the tag
    git tag | grep -q "$version" || die "The version '$version' doesn't have a tag, so I refuse to bump version that would make it the 'old' version."
    # check that tarball with current version is already on GitHub.
    curl --output /dev/null --silent --head --fail "$GITHUB_ROOT/releases/download/$version/openscap-$version.tar.gz" || die "There is not the tarball with '$version' available for download from GitHub."
}


release_to_git()
{
    git tag | grep -q "$version" && die "Something is wrong - there already is a tag $version"
    git tag "$version"
    git push
    git push --tags
}


# Args:
# $1: The filename
# $2: The next version
bump_release_in_release_script()
{
    sed -i "s/\(^previous_version=\).*/\1$version/" "$1"
    sed -i "s/\(^version=\).*/\1$2/" "$1"
}


# Args:
# $1: The filename
# $2: The next version
bump_release_in_configure()
{
    sed -i "s/^\(AC_INIT(\[openscap\],\s*\[\)$version/\1$2/" "$1"
}


# Args:
# $1: The next version
bump_release()
{
    test $# -lt 2 || die "Provide the version number as an argument"
    check_that_bump_is_appropriate
    bump_release_in_configure "$OSCAP_REPO_ROOT/configure.ac" "$1"
    bump_release_in_configure "$OSCAP_REPO_ROOT/ac_probes/configure.ac.tpl" "$1"
    bump_release_in_release_script "$FILE_WITH_VERSIONS" "$1"
    git diff
    git add "$OSCAP_REPO_ROOT/configure.ac" "$OSCAP_REPO_ROOT/ac_probes/configure.ac.tpl" "$FILE_WITH_VERSIONS"
    printf "Commit with this message:\n%s\n\n%s\n" "Version bump after release." "Next release from the maint-${1%.*} branch will be $1"
}

# Args:
# $1: Username
# $2: Repository owner
upload_to_git()
{
    local _username="$1" _repo_owner="$2"
    make_dist > /dev/null 2> /dev/null
    curl -u ":$GITHUB_TOKEN"  --data '{"tag_name":"'$version'"}' "https://api.github.com/repos/$_repo_owner/openscap/releases"

    xdg-open "$GITHUB_ROOT/$_repo_owner/openscap/releases/$version"
}

# Args:
# $1: Token
# $2: Repository owner
# $3: The new version
flip_milestones()
{
    "./$SCRIPT_DIR/move-milestones.py" --owner "$2" --api-token "$1" "$version" "$3"
}


make_news_template()
{
    local _right_boundary=75 _oscap_version_string _oscap_version_length _news_template=NEWS.template _recent_authors="/tmp/recent_authors" _new_authors="/tmp/new_authors"

    get_new_authors "$_recent_authors" "$_new_authors"

    _oscap_version_string="openscap-$version"
    _oscap_version_length="$(echo "$_oscap_version_string" | wc -m)"
    printf "%s%$((_right_boundary - _oscap_version_length))s\n" "$_oscap_version_string" "$(date +%d-%m-%Y)" > "$_news_template"

    printf "  - %s\n" "Stats@STATS@" "New features" "Maintenance" >> "$_news_template"
    stats=
    stats="$stats\n    - $(git log "$previous_version".. | grep '^Author' | wc -l) commits from $(cat "$_recent_authors" | wc -l) distinct persons."
    stats="$stats\n    - $(cat "$_new_authors" | wc -l) new contributors."
    stats="$stats\n    - $("$SCRIPT_DIR/query-milestones.py" --auth-token "$GITHUB_TOKEN" "$version" issues-closed | wc -l) issues closed, $("$SCRIPT_DIR/query-milestones.py" --auth-token "$GITHUB_TOKEN" "$version" prs-merged | wc -l) PRs merged."
    sed -i "s/@STATS@/$stats/" "$_news_template"

    echo >> "$_news_template"
    echo >> "$_news_template"
    echo "####################### Summaries #######################" >> "$_news_template"
    echo >> "$_news_template"

    echo "Closed issues:" >> "$_news_template"
    "$SCRIPT_DIR/query-milestones.py" --auth-token "$GITHUB_TOKEN" "$version" issues-closed | sed -e 's/^/  - /' >> "$_news_template"

    echo "Merged PRs:" >> "$_news_template"
    "$SCRIPT_DIR/query-milestones.py" --auth-token "$GITHUB_TOKEN" "$version" prs-merged | sed -e 's/^/  - /' >> "$_news_template"

    echo "New authors:" >> "$_news_template"
    sed -e 's/^/  - /' "$_new_authors" >> "$_news_template"

    echo "Recent authors:" >> "$_news_template"
    sed -e 's/^/  - /' "$_recent_authors" >> "$_news_template"

    rm -- "$_recent_authors" "$_new_authors"
}


check_release_is_ok()
{
    grep -q "^openscap-$version" "$OSCAP_REPO_ROOT/NEWS" || die "The version '$version' isn't mentioned in the NEWS file."
    git log -1 | grep -q "^\s*openscap-$version$" || die "The openscap release commit is not the previous commit."
}


# Args:
# $1: The future version number
release_to_git_and_bump_release()
{
    local _new_version="$1"
    test -n "$GITHUB_TOKEN" || die "We don't know your GitHub token, so we can't proceed. Get one on https://github.com/settings/tokens and put it in the .env file, so it contains the line GITHUB_TOKEN='<your token here>'"
    test "$1" = "$version" && die "The new version is the same as current version, I am not doing anything."
    check_release_is_ok
    release_to_git
    # upload_to_git  # to be done when https://github.com/PyGithub/PyGithub/pull/525 is merged.
    flip_milestones --auth-token "$GITHUB_TOKEN" openscap "$_new_version"
    bump_release "$_new_version"
}
