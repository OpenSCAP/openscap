/*
 * Shared setup for the OpenSCAP OVAL *probe* fuzz harnesses.
 *
 * These harnesses fuzz the data an OVAL probe parses out of the *scanned
 * filesystem* (e.g. /etc/passwd, xinetd config files, /proc/net/tcp, a text
 * file matched by a regex) rather than a SCAP XML document. They reach the
 * probe's parsing code by #include-ing the probe's .c file directly (the same
 * trick tests/probes/xinetd/test_probe_xinetd.c uses) and calling the static
 * parser, so they must be built with -DENABLE_PROBES=ON.
 *
 * This header provides:
 *   - PROBE_FUZZ_INIT(): one-time process init (silence libxml2, oscap_init()).
 *   - a reusable tmpfs-backed temp file for parsers that insist on a path.
 *   - a minimal probe_ctx + icache for parsers that emit via probe_item_collect.
 *
 * NOTE: fmemopen() and the /dev/shm temp file are glibc/Linux specific. That is
 * fine: the probes targeted here are Linux/UNIX probes and the fuzzing build
 * runs on Linux.
 */
#ifndef OPENSCAP_PROBE_FUZZ_COMMON_H
#define OPENSCAP_PROBE_FUZZ_COMMON_H

#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xmlerror.h>

#include "oscap.h"

/* one-time process initialization (mirrors fuzz/fuzz_common.h) */
static inline void probe_fuzz_init_once(void)
{
	xmlSetGenericErrorFunc(NULL, NULL);
	xmlSetStructuredErrorFunc(NULL, NULL);
	oscap_init();
}

#define PROBE_FUZZ_INIT()                          \
	do {                                       \
		static int _pf_inited = 0;         \
		if (!_pf_inited) {                 \
			probe_fuzz_init_once();    \
			_pf_inited = 1;            \
		}                                  \
	} while (0)

/*
 * A single tmpfs-backed file reused across iterations. Probes such as xinetd,
 * inetlisteningservers and textfilecontent54 open a *path* and read it
 * themselves, so the fuzz bytes have to live in a real file. Reusing one file
 * (truncate + rewrite each iteration) avoids per-iteration mkstemp()/unlink()
 * churn that would otherwise dominate runtime and exhaust inodes.
 *
 * The path includes the PID so that libFuzzer's -fork children (which all run
 * the same binary) do not race on a single shared file and corrupt each
 * other's input.
 */
#define PROBE_FUZZ_TMPDIR "/dev/shm"

/* Returns a process-unique temp file path (stable for the life of the process). */
static inline const char *probe_fuzz_tmppath(void)
{
	static char path[64];
	if (path[0] == '\0')
		snprintf(path, sizeof(path), "%s/oscap_probe_fuzz_input.%ld",
		         PROBE_FUZZ_TMPDIR, (long)getpid());
	return path;
}

/* Write the fuzz bytes to the reusable temp file; return its path or NULL. */
static inline const char *probe_fuzz_write_tmpfile(const uint8_t *data, size_t size)
{
	const char *path = probe_fuzz_tmppath();
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
		return NULL;
	size_t off = 0;
	while (off < size) {
		ssize_t w = write(fd, data + off, size - off);
		if (w <= 0) {
			close(fd);
			return NULL;
		}
		off += (size_t)w;
	}
	close(fd);
	return path;
}

/* NUL-terminated heap copy of the fuzz input, for buffer/line parsers. */
static inline char *probe_fuzz_cstr(const uint8_t *data, size_t size)
{
	char *s = malloc(size + 1);
	if (s == NULL)
		return NULL;
	memcpy(s, data, size);
	s[size] = '\0';
	return s;
}

#endif /* OPENSCAP_PROBE_FUZZ_COMMON_H */
