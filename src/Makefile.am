pkginclude_HEADERS = st_dict.h \
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
libstutils_la_LIBADD = -lm

check_PROGRAMS = tests/st-utils-test
tests_st_utils_test_SOURCES = tests/st-utils-test.c
tests_st_utils_test_LDADD = libstutils.la

TESTS = $(check_PROGRAMS)

include $(top_srcdir)/aminclude_static.am

@VALGRIND_CHECK_RULES_LOCAL@

.PHONY: check-git-commit

all: check-git-commit

check-git-commit:
	@if grep '^#define ST_GIT_COMMIT' st_macro.h | awk '{if($$3 ~ /[0-9a-f]{40}/){exit 1} else {exit 0}}'; then \
	echo "No commit version set. Do you forget to include .gitconfig in your local config."; exit 1; fi
