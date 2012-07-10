#
# Use this to run local oscap tool build

# Usage example:
# $ ./oscap-local.sh gdb --args .libs/oscap xccdf eval ...
if [ $# -eq 0 ]; then
	echo "Usage: \"$ ./oscap-local.sh .libs/oscap xccdf eval ...\"";
fi

PREFIX=".."

# Preload testing library that accept various environment variables
export LD_PRELOAD="$PREFIX/src/.libs/libopenscap_testing.so"
export LD_LIBRARY_PATH="$PREFIX/src/.libs"

# Set paths to probes, schemas and transformations
export OVAL_PROBE_DIR="$PREFIX/src/OVAL/probes"
export OSCAP_SCHEMA_PATH="$PREFIX/schemas"
export OSCAP_XSLT_PATH="$PREFIX/xsl"

# Execution part
exec "$@"

