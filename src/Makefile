PROJECT = stutils

include flags.mk

REV_INC = st_macro.h
GIT_COMMIT_MACRO = ST_GIT_COMMIT

INCS = st_dict.h \
       st_alphabet.h \
       st_utils.h \
       st_conf.h \
       st_log.h \
       st_queue.h \
       st_stack.h \
       st_heap.h \
       st_net.h \
       st_io.h \
       st_opt.h \
       st_semaphore.h

SRCS = st_dict.c \
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

TESTS = tests/st-utils-test

VAL_TESTS = tests/st-utils-test

.PHONY: all
all:

include common_static.mk
include common.mk
