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

set(FUZZ_TARGETS
	scap_parse_fuzzer
	xccdf_policy_fuzzer
	validate_fuzzer
	arf_fuzzer
	xccdf_tailoring_fuzzer
)

# Probe-content harnesses: these fuzz the data an OVAL probe parses out of the
# scanned filesystem (xinetd configs, /etc/passwd, /proc/net/tcp, text files,
# ...). They #include a probe's .c directly to reach its static parser, so they
# only build when probes are compiled into the library (-DENABLE_PROBES=ON) and
# they need the probe headers plus a few helper sources that are not exported
# from libopenscap.so (the same set tests/probes/xinetd uses).
if(ENABLE_PROBES)
	set(PROBE_FUZZ_INCLUDE_DIRS
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/public"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/public"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/rbt"
		"${CMAKE_SOURCE_DIR}/src/common"
	)
	set(PROBE_FUZZ_EXTRA_SOURCES
		"${CMAKE_SOURCE_DIR}/src/common/bfind.c"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/rbt/rbt_common.c"
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/rbt/rbt_str.c"
		"${CMAKE_SOURCE_DIR}/src/common/oscap_pcre.c"
	)

	# add_fuzzer_probe(<name> <source> [extra sources...]) is add_fuzzer() plus
	# the probe include dirs and the helper sources needed to compile an
	# #include-d probe .c. Extra per-harness sources are passed via ARGN.
	#
	# --gc-sections drops the probe's own *_probe_main (and the static helpers
	# only it reaches, e.g. collect_item): we drive the parser functions
	# directly, and those entry points would otherwise pull in non-exported
	# probe-runtime symbols that are not linked here.
	function(add_fuzzer_probe name source)
		add_executable(${name} ${source} ${PROBE_FUZZ_EXTRA_SOURCES} ${ARGN})
		target_include_directories(${name} PRIVATE ${FUZZ_INCLUDE_DIRS} ${PROBE_FUZZ_INCLUDE_DIRS})
		target_link_libraries(${name} openscap)
		target_compile_options(${name} PRIVATE -fsanitize=fuzzer -ffunction-sections -fdata-sections)
		target_link_options(${name} PRIVATE -fsanitize=fuzzer -Wl,--gc-sections)
	endfunction()

	add_fuzzer_probe(xinetd_probe_fuzzer xinetd_probe_fuzzer.c)   # xinetd config parser
	add_fuzzer_probe(routingtable_probe_fuzzer routingtable_probe_fuzzer.c
		"${CMAKE_SOURCE_DIR}/src/OVAL/probes/SEAP/generic/strto.c")  # /proc/net/route line parser
	add_fuzzer_probe(shadow_probe_fuzzer shadow_probe_fuzzer.c)  # /etc/shadow hash-method parser
	list(APPEND FUZZ_TARGETS xinetd_probe_fuzzer routingtable_probe_fuzzer shadow_probe_fuzzer)

	# Some probe parsers (textfilecontent54's process_file, inetlisteningservers'
	# read_tcp) call non-exported probe helpers directly -- probe_entobj_cmp (->
	# the OVAL comparison code), the item cache, oval_fts -- which --gc-sections
	# cannot drop because the code we drive uses them. The shared library hides
	# those symbols (C_VISIBILITY_PRESET hidden), so instead of linking the .so
	# we link the library's *object* files (visibility only affects the dynamic
	# symbol table, not static linking) and link the openscap target purely to
	# inherit its external dependencies (libxml2, pcre2, ...). Mirrors the
	# OBJECTS_TO_LINK_AGAINST list in src/CMakeLists.txt.
	set(PROBE_FUZZ_FULL_OBJECTS
		$<TARGET_OBJECTS:common_object>
		$<TARGET_OBJECTS:cpe_object>
		$<TARGET_OBJECTS:ds_object>
		$<TARGET_OBJECTS:oscapsource_object>
		$<TARGET_OBJECTS:oval_object>
		$<TARGET_OBJECTS:ovaladt_object>
		$<TARGET_OBJECTS:ovalcmp_object>
		$<TARGET_OBJECTS:ovalresults_object>
		$<TARGET_OBJECTS:rbt_object>
		$<TARGET_OBJECTS:xccdf_object>
		$<TARGET_OBJECTS:xccdfPolicy_object>
		$<TARGET_OBJECTS:probe_object>
		$<TARGET_OBJECTS:seap_object>
		$<TARGET_OBJECTS:independent_probes_object>
		$<TARGET_OBJECTS:unix_probes_object>
	)
	foreach(opt_obj compat_object yamlfilter_object crapi_object linux_probes_object)
		if(TARGET ${opt_obj})
			list(APPEND PROBE_FUZZ_FULL_OBJECTS $<TARGET_OBJECTS:${opt_obj}>)
		endif()
	endforeach()

	# add_fuzzer_probe_full(<name> <source> [extra sources...]) for harnesses that
	# need the library's non-exported internals: links the composing object files
	# (all symbols available at static-link time) plus the openscap target for its
	# external link interface. No --gc-sections here -- all referenced symbols are
	# present, so nothing needs to be pruned.
	function(add_fuzzer_probe_full name source)
		add_executable(${name} ${source} ${PROBE_FUZZ_FULL_OBJECTS} ${ARGN})
		target_include_directories(${name} PRIVATE ${FUZZ_INCLUDE_DIRS} ${PROBE_FUZZ_INCLUDE_DIRS})
		# Link the external deps openscap pulls in (libxml2, pcre2, ...), but NOT
		# the openscap shared object itself: linking the .so on top of the same
		# object files would define every global twice (an ASan ODR violation).
		target_link_libraries(${name} $<TARGET_PROPERTY:openscap,LINK_LIBRARIES>)
		target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
		target_link_options(${name} PRIVATE -fsanitize=fuzzer)
	endfunction()

	add_fuzzer_probe_full(textfilecontent54_probe_fuzzer textfilecontent54_probe_fuzzer.c) # text file + PCRE reader (whole-file)
	add_fuzzer_probe_full(textfilecontent_probe_fuzzer textfilecontent_probe_fuzzer.c)     # legacy text file + PCRE reader (per-line)
	list(APPEND FUZZ_TARGETS textfilecontent54_probe_fuzzer textfilecontent_probe_fuzzer)
	if(ENABLE_PROBES_LINUX)
		add_fuzzer_probe_full(inetlisteningservers_probe_fuzzer inetlisteningservers_probe_fuzzer.c) # /proc/net/{tcp,udp} parser
		add_fuzzer_probe_full(iflisteners_probe_fuzzer iflisteners_probe_fuzzer.c) # /proc/net/packet parser
		list(APPEND FUZZ_TARGETS inetlisteningservers_probe_fuzzer iflisteners_probe_fuzzer)
	endif()
endif()

# Convenience target to build them all: `cmake --build . --target fuzzers`
add_custom_target(fuzzers DEPENDS ${FUZZ_TARGETS})
