GCC = gcc

OUTINC_DIR = ../include/
BIN_DIR=../bin
OUTLIB_DIR=../lib

OBJ_DIR=./.objs
DEP_DIR=./.deps

include flags.mk
include templates.mk

PROJECT = stutils
CONFIG_INC = st_macro.h
OUT_CONFIG = $(OUTINC_DIR)/$(PROJECT)/$(CONFIG_INC)

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

OBJS = st_dict.o \
       st_alphabet.o \
       st_utils.o \
       st_conf.o \
       st_log.o \
       st_queue.o \
       st_stack.o \
       st_heap.o \
       st_net.o \
       st_io.o \
       st_opt.o \
       st_semaphore.o

ifeq ($(UNAME_S),Darwin)
TARGET_SO = $(OUTLIB_DIR)/libstutils.dylib
SO_FLAGS = -dynamiclib -install_name $(shell pwd)/$(TARGET_SO)
else
TARGET_SO = $(OUTLIB_DIR)/libstutils.so
SO_FLAGS = -shared
endif

TEST_BINS = tests/st-utils-test

.PHONY: all config inc clean lint-check
.PHONY: test val-test
.PHONY: prepare-doc gen-doc publish-doc

all: inc $(TARGET_SO)

# always check revision
config:
	@./replace_revision.sh $(CONFIG_INC) $(OUT_CONFIG)

# copy config.h when updated
$(OUT_CONFIG): $(CONFIG_INC)
	./replace_revision.sh $(CONFIG_INC) $(OUT_CONFIG) 1

OUT_INCS = $(addprefix $(OUTINC_DIR)/$(PROJECT)/,$(INCS))

inc: $(OUT_INCS) config $(OUT_CONFIG)

$(foreach f,$(INCS),$(eval $(call copy_inc,$f,$(OUTINC_DIR)/$(PROJECT)/$f)))

$(TARGET_SO) : $(addprefix $(OBJ_DIR)/,$(OBJS))
	@mkdir -p "$(dir $@)"
	$(GCC) $(CFLAGS) $(LDFLAGS) $(SO_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o : %.c %.h
	@mkdir -p "$(dir $@)"
	$(GCC) $(CFLAGS) -fPIC -c -o $@ $<

$(BIN_DIR)/%.o : %.c %.h
	@mkdir -p "$(dir $@)"
	$(GCC) $(CFLAGS) -c -o $@ $^ 


lint-check : *.h *.cpp *.c
	splint +posixlib +D__gnuc_va_list=int -fileextensions $^ 

clean:
	rm -rf ${OBJ_DIR}
	rm -rf ${DEP_DIR}
	rm -rf ${BIN_DIR}
	rm -rf ${OUTLIB_DIR}
	rm -rf ${OUTINC_DIR}
	rm -f tags cscope.*

TEST_TARGETS = $(addprefix $(OBJ_DIR)/,$(TEST_BINS))

test: inc $(TARGET_SO) $(TEST_TARGETS)
	@result=0; \
     for x in $(TEST_TARGETS); do \
       printf "Running $$x ..."; \
       ./$$x >/dev/null 2>&1; \
       if [ $$? -ne 0 ]; then \
         echo "... FAIL $$x"; \
         result=1; \
       else \
         echo "... SUCCESS"; \
       fi; \
     done; \
     exit $$result

val-test: inc $(TARGET_SO) $(TEST_TARGETS)
	@result=0; \
     for x in $(TEST_TARGETS); do \
       printf "Valgrinding $$x ..."; \
       valgrind ./$$x; \
       if [ $$? -ne 0 ]; then \
         echo "... FAIL $$x"; \
         result=1; \
       else \
         echo "... SUCCESS"; \
       fi; \
     done; \
     exit $$result

# complie and link tests
$(OBJ_DIR)/%: %.c
	@mkdir -p "$(dir $@)"
	$(GCC) $(CFLAGS) -UNDEBUG -o $@ $< -L$(OUTLIB_DIR) -l$(PROJECT) $(LDFLAGS) 

gen-doc: prepare-doc
	rm -rf html/*
	doxygen doc/Doxyfile > /dev/null

prepare-doc:
	mkdir -p html
	if [ ! -d html/.git ]; then \
      git clone git@github.com:wantee/$(PROJECT).git html; \
    fi
	cd html && \
    git checkout gh-pages

publish-doc: gen-doc
	cd html && \
	git add * && \
	git commit -a -m"generate doc on $(shell date)" && \
	git push origin gh-pages
