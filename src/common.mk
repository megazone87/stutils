DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.Td
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d
COMPILE_bin = $(CC) $(DEPFLAGS) $(CFLAGS) -L$(OUTLIB_DIR) -l$(PROJECT) $(LDFLAGS) -Wl,-rpath,$(abspath $(OUTLIB_DIR))
COMPILE_test_bin = $(CC) $(DEPFLAGS) $(CFLAGS) -UNDEBUG -L$(OUTLIB_DIR) -l$(PROJECT) $(LDFLAGS) -Wl,-rpath,$(abspath $(OUTLIB_DIR))

ifeq ($(shell uname -s),Darwin)
TARGET_SO = $(OUTLIB_DIR)/lib$(PROJECT).dylib
SO_FLAGS = -dynamiclib -install_name $(abspath $(TARGET_SO))
else
TARGET_SO = $(OUTLIB_DIR)/lib$(PROJECT).so
SO_FLAGS = -shared
endif
TARGET_BINS = $(addprefix $(OUTBIN_DIR)/,$(BINS))

OUT_REV = $(OUTINC_DIR)/$(PROJECT)/$(REV_INC)
OUT_INCS = $(addprefix $(OUTINC_DIR)/$(PROJECT)/,$(INCS))

.PHONY: all inc rev lint-check
.PHONY: test val-test
.PHONY: clean clean-bin
.PHONY: prepare-doc gen-doc publish-doc

all: inc $(TARGET_SO) $(TARGET_BINS)

REPLACE_REVISION=./.replace_revision.sh
define REPLACE_REVISION_CMD
#!/bin/sh

str=$$3
if [ -e "$$2" ]; then
rev_in=`grep "$$3" $$2 | awk '{print $$3}' | tr -d '"'`
else
rev_in=""
fi
rev=`git rev-parse HEAD`;

if [ -n "$$4" ] || [ "$$rev" != "$$rev_in" ]; then
  mkdir -p `dirname $$2`
  sed -E "s/#define[[:space:]]+$$3[[:space:]].*/#define $$3 \"$$rev\"/" $$1 > $$2
fi
endef
export REPLACE_REVISION_CMD

$(REPLACE_REVISION) :
	@echo "$$REPLACE_REVISION_CMD" > $@
	@chmod a+x $@

# always check revision
rev: $(REPLACE_REVISION)
	@$(REPLACE_REVISION) $(REV_INC) $(OUT_REV) $(GIT_COMMIT_MACRO)

# copy REV_HEADER when updated
$(OUT_REV): $(REPLACE_REVISION) $(REV_INC)
	$(REPLACE_REVISION) $(REV_INC) $(OUT_REV) $(GIT_COMMIT_MACRO) force

inc: $(OUT_INCS) rev $(OUT_REV)

$(OUT_INCS) : $(OUTINC_DIR)/$(PROJECT)/%.h : %.h
	@mkdir -p "$(dir $@)"
	cp "$<" "$@"

$(TARGET_SO) : $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
	@mkdir -p "$(dir $@)"
	$(LINK.o) $(SO_FLAGS) $^ -o $@

$(OBJ_DIR)/%.o : %.c $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.c) $(OUTPUT_OPTION) -fPIC $<
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o : %.cc $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.cc) $(OUTPUT_OPTION) -fPIC $<
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o : %.cxx $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.cc) $(OUTPUT_OPTION) -fPIC $<
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o : %.cpp $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.cc) $(OUTPUT_OPTION) -fPIC $<
	$(POSTCOMPILE)

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(patsubst %,$(DEP_DIR)/%.d,$(basename $(SRCS)))

$(TARGET_BINS) : $(OUT_REV) $(TARGET_SO)
$(TARGET_BINS) : $(OUTBIN_DIR)/% : %.c $(DEP_DIR)/%.d
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE_bin) -o $@ $<
	$(POSTCOMPILE)

-include $(patsubst %,$(DEP_DIR)/%.d,$(basename $(BINS)))

TARGET_TESTS = $(addprefix $(OBJ_DIR)/,$(TESTS))

$(TARGET_TESTS) : $(OUT_REV) $(TARGET_SO)
$(TARGET_TESTS) : $(OBJ_DIR)/% : %.c $(DEP_DIR)/%.d
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE_test_bin) -o $@ $<
	$(POSTCOMPILE)

-include $(patsubst %,$(DEP_DIR)/%.d,$(basename $(TESTS)))

test: $(TARGET_TESTS)
	@result=0; \
     for x in $(TARGET_TESTS); do \
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

TARGET_VAL_TESTS = $(addprefix $(OBJ_DIR)/,$(VAL_TESTS))

val-test: $(TARGET_VAL_TESTS)
	@result=0; \
     for x in $(TARGET_VAL_TESTS); do \
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

lint-check : *.h *.cpp *.c
	splint +posixlib +D__gnuc_va_list=int -fileextensions $^ 

ifdef BINS

clean-bin:
	rm -f $(addprefix $(OUTBIN_DIR)/,$(BINS))
	rm -rf $(addsuffix .dSYM,$(addprefix $(OUTBIN_DIR)/,$(BINS)))
	rm -rf $(addprefix $(OUTBIN_DIR)/,$(filter-out ./,$(dir $(BINS))))

else

clean-bin:

endif

clean: clean-bin
	rm -rf $(OBJ_DIR)
	rm -rf $(DEP_DIR)
	rm -rf $(OUTLIB_DIR)
	rm -rf $(OUTINC_DIR)
	rm -rf $(REPLACE_REVISION)
	rm -f tags cscope.*
