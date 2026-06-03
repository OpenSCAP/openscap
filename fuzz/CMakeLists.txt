# libFuzzer harnesses for SCAP parsing / processing.
#
# Enabled with -DENABLE_FUZZING=ON. Requires a Clang toolchain (libFuzzer ships
# with clang). When enabled, the whole library is compiled with the libFuzzer
# coverage instrumentation plus AddressSanitizer/UndefinedBehaviorSanitizer (set
# from the top-level CMakeLists), and each harness is linked with
# -fsanitize=fuzzer to pull in the libFuzzer driver/main.

set(FUZZ_INCLUDE_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/src/common/public"
	"${CMAKE_SOURCE_DIR}/src/source/public"
	"${CMAKE_SOURCE_DIR}/src/DS/public"
	"${CMAKE_SOURCE_DIR}/src/XCCDF/public"
	"${CMAKE_SOURCE_DIR}/src/XCCDF_POLICY/public"
	"${CMAKE_SOURCE_DIR}/src/CPE/public"
	"${CMAKE_SOURCE_DIR}/src/OVAL/public"
	"${LIBXML2_INCLUDE_DIR}"
)

# add_fuzzer(<name> <source>) builds one libFuzzer executable linked against the
# instrumented library.
function(add_fuzzer name source)
	add_executable(${name} ${source})
	target_include_directories(${name} PRIVATE ${FUZZ_INCLUDE_DIRS})
	target_link_libraries(${name} openscap)
	target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
	target_link_options(${name} PRIVATE -fsanitize=fuzzer)
endfunction()

add_fuzzer(scap_parse_fuzzer     scap_parse_fuzzer.c)     # dispatch-by-type parser
add_fuzzer(xccdf_policy_fuzzer    xccdf_policy_fuzzer.c)   # XCCDF policy/profile layer
add_fuzzer(validate_fuzzer        validate_fuzzer.c)       # XSD + Schematron validation
add_fuzzer(arf_fuzzer             arf_fuzzer.c)            # ARF / result data stream (RDS)
add_fuzzer(xccdf_tailoring_fuzzer xccdf_tailoring_fuzzer.c) # XCCDF tailoring

# Convenience target to build them all: `cmake --build . --target fuzzers`
add_custom_target(fuzzers DEPENDS
	scap_parse_fuzzer
	xccdf_policy_fuzzer
	validate_fuzzer
	arf_fuzzer
	xccdf_tailoring_fuzzer
)
