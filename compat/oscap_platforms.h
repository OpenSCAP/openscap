/*
 *
 * Copyright 2018 OpenSCAP Contributors. See COPYING for License.
 *
 */

#undef OS_FREEBSD
#undef OS_LINUX
#undef OS_SOLARIS
#undef OS_SUNOS
#undef OS_WINDOWS
#undef OS_AIX
#undef OS_APPLE
#undef OS_OSX

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
# define OS_FREEBSD
#elif defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
# define OS_LINUX
#elif defined(sun) || defined(__sun)
# if defined(__SVR4) || defined(__svr4__)
#  define OS_SOLARIS
# else
#  define OS_SUNOS
# endif
#elif defined(_WIN32) || defined(_WIN64)
# define OS_WINDOWS
#elif defined(_AIX)
# define OS_AIX
#elif defined(__APPLE__)
# define OS_APPLE
# if defined(Macintosh) || defined(macintosh) || defined(__APPLE__) && defined(__MACH__)
#  define OS_OSX
# endif
#else
# error "Sorry, your OS isn't supported."
#endif

