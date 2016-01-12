include_HEADERS = st_dict.h \
       st_alphabet.h \
       st_utils.h \
       st_conf.h \
       st_log.h \
       st_macro.h \
       st_queue.h \
       st_stack.h \
       st_heap.h \
       st_net.h \
       st_io.h \
       st_opt.h \
       st_semaphore.h

lib_LTLIBRARIES = libstutils.la
libstutils_la_SOURCES = st_dict.c \
       st_alphabet.c \
       st_utils.c \
       st_conf.c \
       st_log.c \
       st_queue.c \
       st_stack.c \
       st_heap.c \
       st_net.c \
       st_io.c \
       st_opt.c \
       st_semaphore.c

AM_CFLAGS = -m64 -pipe

check_PROGRAMS = st-utils-test
st_utils_test_SOURCES = st-utils-test.c
st_utils_test_LDADD = libstutils.la

TESTS = $(check_PROGRAMS)
