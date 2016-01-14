#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Require automake 1.15

AC_PREREQ([2.69])
AC_INIT([stutils], [0.0.1], [wantee.wang@gmail.com], [stutils], [https://github.com/wantee/stutils])

AC_CONFIG_SRCDIR([src/st_macro.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror -Wno-portability foreign])
AM_EXTRA_RECURSIVE_TARGETS([splint-check])

AM_PROG_AR
LT_INIT

DX_INIT_DOXYGEN_ADD(stutils, src/doc/Doxyfile, doc)
    DX_DOXYGEN_FEATURE(ON)
    DX_DOT_FEATURE(OFF)
    DX_HTML_FEATURE(ON)
    DX_CHM_FEATURE(OFF)
    DX_CHI_FEATURE(OFF)
    DX_MAN_FEATURE(OFF)
    DX_RTF_FEATURE(OFF)
    DX_XML_FEATURE(OFF)
    DX_PDF_FEATURE(OFF)
    DX_PS_FEATURE(OFF)

# Checks for programs.
AC_PROG_CC
AX_PROG_SPLINT_RECURSIVE

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h limits.h netdb.h netinet/in.h stdint.h stdlib.h string.h strings.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([bzero gethostbyname gettimeofday localtime_r memmove memset mkdir socket sqrt strcasecmp strchr strerror strncasecmp strpbrk strrchr])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
