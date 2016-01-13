ACLOCAL_AMFLAGS = -I m4

include aminclude_static.am
include aminclude.am

include_HEADERS = src/st_dict.h \
       src/st_alphabet.h \
       src/st_utils.h \
       src/st_conf.h \
       src/st_log.h \
       src/st_macro.h \
       src/st_queue.h \
       src/st_stack.h \
       src/st_heap.h \
       src/st_net.h \
       src/st_io.h \
       src/st_opt.h \
       src/st_semaphore.h

lib_LTLIBRARIES = libstutils.la
libstutils_la_SOURCES = src/st_dict.c \
       src/st_alphabet.c \
       src/st_utils.c \
       src/st_conf.c \
       src/st_log.c \
       src/st_queue.c \
       src/st_stack.c \
       src/st_heap.c \
       src/st_net.c \
       src/st_io.c \
       src/st_opt.c \
       src/st_semaphore.c
libstutils_la_LIBADD = -lm

AM_CFLAGS = -m64 -pipe

check_PROGRAMS = src/tests/st-utils-test
src_tests_st_utils_test_SOURCES = src/tests/st-utils-test.c
src_tests_st_utils_test_LDADD = libstutils.la

TESTS = $(check_PROGRAMS)

EXTRA_DIST = LICENSE README.md doc/html
