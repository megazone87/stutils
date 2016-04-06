DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.Td
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
ifndef STATIC_LINK
COMPILE.c += -fPIC
COMPILE.cc += -fPIC
endif
LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
POSTCOMPILE = mv -f $(DEP_DIR)/$*.Td $(DEP_DIR)/$*.d
COMPILE_bin.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -o $@ $< -l$(PROJECT) $(LDFLAGS)
COMPILE_bin.cc = $(CC) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< -L$(OUTLIB_DIR) -l$(PROJECT) $(LDFLAGS)
COMPILE_test_bin.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -UNDEBUG -o $@ $<  -L$(OUTLIB_DIR) -l$(PROJECT) $(LDFLAGS)
COMPILE_test_bin.cc = $(CC) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -UNDEBUG -o $@ $< -L$(OUTLIB_DIR) -l$(PROJECT) $(LDFLAGS)
ifdef STATIC_LINK
COMPILE_bin.c += -L$(OBJ_DIR) -Wl,-rpath,'$$ORIGIN/../lib'
COMPILE_bin.cc += -L$(OBJ_DIR) -Wl,-rpath,'$$ORIGIN/../lib'
COMPILE_test_bin.c += -L$(OBJ_DIR)
COMPILE_test_bin.cc += -L$(OBJ_DIR)
else
COMPILE_bin.c += -L$(OUTLIB_DIR) -Wl,-rpath,$(abspath $(OUTLIB_DIR)) -Wl,-rpath,'$$ORIGIN/../lib'
COMPILE_bin.cc += -L$(OUTLIB_DIR) -Wl,-rpath,$(abspath $(OUTLIB_DIR)) -Wl,-rpath,'$$ORIGIN/../lib'
COMPILE_test_bin.c += -L$(OUTLIB_DIR) -Wl,-rpath,$(abspath $(OUTLIB_DIR))
COMPILE_test_bin.cc += -L$(OUTLIB_DIR) -Wl,-rpath,$(abspath $(OUTLIB_DIR))
endif

ifdef STATIC_LINK
  TARGET_LIB=$(OBJ_DIR)/lib$(PROJECT).a
else
  ifeq ($(shell uname -s),Darwin)
  TARGET_LIB = $(OUTLIB_DIR)/lib$(PROJECT).dylib
  SO_FLAGS = -dynamiclib -install_name $(abspath $(TARGET_LIB))
  else
  TARGET_LIB = $(OUTLIB_DIR)/lib$(PROJECT).so
  SO_FLAGS = -shared
  endif
endif
TARGET_BINS = $(addprefix $(OUTBIN_DIR)/,$(BINS))

ifdef REV_INC
OUT_REV = $(OUTINC_DIR)/$(PROJECT)/$(REV_INC)
else
OUT_REV =
endif
OUT_INCS = $(addprefix $(OUTINC_DIR)/$(PROJECT)/,$(INCS))

.PHONY: $(PREFIX)all $(PREFIX)inc $(PREFIX)rev
.PHONY: $(PREFIX)test $(PREFIX)val-test
.PHONY: $(PREFIX)clean $(PREFIX)clean-bin

$(PREFIX)all: $(PREFIX)inc $(TARGET_LIB) $(TARGET_BINS)

ifdef REV_INC

# always check revision
$(PREFIX)rev: $(REPLACE_REVISION)
	@$(REPLACE_REVISION) $(REV_INC) $(OUT_REV) $(GIT_COMMIT_MACRO)

# copy REV_HEADER when updated
$(OUT_REV): $(REPLACE_REVISION) $(REV_INC)
	$(REPLACE_REVISION) $(REV_INC) $(OUT_REV) $(GIT_COMMIT_MACRO) force

endif

$(PREFIX)inc: $(OUT_INCS) $(PREFIX)rev $(OUT_REV)

$(OUT_INCS) : $(OUTINC_DIR)/$(PROJECT)/%.h : %.h
	@mkdir -p "$(dir $@)"
	cp "$<" "$@"

ifdef STATIC_LINK

$(TARGET_LIB) : $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
	@mkdir -p "$(dir $@)"
	ar rcs $@ $^

else

$(TARGET_LIB) : $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))
	@mkdir -p "$(dir $@)"
	$(LINK.o) $(SO_FLAGS) $^ -o $@

endif

$(OBJ_DIR)/%.o : %.c $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o : %.cc $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o : %.cxx $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJ_DIR)/%.o : %.cpp $(DEP_DIR)/%.d $(OUT_REV)
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEP_DIR)/%.d: ;
.PRECIOUS: $(DEP_DIR)/%.d

-include $(patsubst %,$(DEP_DIR)/%.d,$(basename $(SRCS)))

$(TARGET_BINS) : $(OUT_REV) $(TARGET_LIB)
$(TARGET_BINS) : $(OUTBIN_DIR)/% : %.c $(DEP_DIR)/%.d
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE_bin.c)
	$(POSTCOMPILE)

-include $(patsubst %,$(DEP_DIR)/%.d,$(basename $(BINS)))

TARGET_TESTS = $(addprefix $(OBJ_DIR)/,$(TESTS))

$(TARGET_TESTS) : $(OUT_REV) $(TARGET_LIB)
$(TARGET_TESTS) : $(OBJ_DIR)/% : %.c $(DEP_DIR)/%.d
	@mkdir -p "$(dir $@)"
	@mkdir -p "$(dir $(DEP_DIR)/$*.d)"
	$(COMPILE_test_bin.c)
	$(POSTCOMPILE)

-include $(patsubst %,$(DEP_DIR)/%.d,$(basename $(TESTS)))

$(PREFIX)test: $(TARGET_TESTS)
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

$(PREFIX)val-test: $(TARGET_VAL_TESTS)
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

ifdef BINS

$(PREFIX)clean-bin:
	rm -f $(addprefix $(OUTBIN_DIR)/,$(BINS))
	rm -rf $(addsuffix .dSYM,$(addprefix $(OUTBIN_DIR)/,$(BINS)))
	rmdir -p $(addprefix $(OUTBIN_DIR)/,$(filter-out ./,$(dir $(BINS)))) 2>/dev/null || true

else

$(PREFIX)clean-bin:

endif

ifdef TESTS

$(PREFIX)clean-test:
	rm -f $(TARGET_TESTS)
	rm -rf $(addsuffix .dSYM,$(TARGET_TESTS))

else

$(PREFIX)clean-test:

endif

$(PREFIX)clean: $(PREFIX)clean-bin clean-static
	rm -rf $(OBJ_DIR)
	rm -rf $(DEP_DIR)
	rm -f $(TARGET_LIB)
	rmdir -p $(OUTLIB_DIR) 2>/dev/null || true
	rm -rf $(OUTINC_DIR)/$(PROJECT)
	rmdir -p $(OUTINC_DIR) 2>/dev/null || true
