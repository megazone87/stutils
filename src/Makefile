UNAME_S := $(shell uname -s)

GCC = gcc
CFLAGS = -Wall -pipe -g -m64
CFLAGS += -march=native -mtune=native -O3
CFLAGS += -DNDEBUG
#CFLAGS += -pg
#LDFLAGS += -pg

BINDIR = ../bin
BIN_OBJ = ./bin_obj
OUTLIBDIR = ../lib
OUTINCDIR = ../include/stutils
SO_OBJ = ./so_obj

LDFLAGS += -lpthread
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
LDFLAGS += -Wl,-rpath,$(shell pwd)/$(OUTLIBDIR)

INC = st_dict.h st_alphabet.h st_utils.h st_conf.h st_log.h \
             st_macro.h st_queue.h st_stack.h st_heap.h st_net.h \
             st_io.h st_opt.h
TARGET_INC = $(addprefix $(OUTINCDIR)/, $(INC))

OBJ_SO = st_dict.o st_alphabet.o st_utils.o st_conf.o st_log.o \
         st_queue.o st_stack.o st_heap.o st_net.o st_io.o st_opt.o

ifeq ($(UNAME_S),Darwin)
TARGET_SO = $(OUTLIBDIR)/libstutils.dylib
SO_FLAGS = -dynamiclib -install_name $(shell pwd)/$(TARGET_SO)
else
TARGET_SO = $(OUTLIBDIR)/libstutils.so
SO_FLAGS = -shared
endif

TEST_BIN = st-utils-test

TARGET_TEST_BIN = $(addprefix $(BINDIR)/, $(TEST_BIN))

all:  preparedir $(TARGET_SO) $(TARGET_INC)

preparedir: 
	@mkdir -p $(BIN_OBJ)
	@mkdir -p $(SO_OBJ)
	@mkdir -p ${BINDIR}
	@mkdir -p ${OUTLIBDIR}
	@mkdir -p ${OUTINCDIR}

$(SO_OBJ)/%.o : %.c 
	$(GCC) $(CFLAGS) -fPIC -c -o $@ $<

$(TARGET_INC) : $(INC)
	cp $^ ${OUTINCDIR}

$(TARGET_SO) : $(addprefix $(SO_OBJ)/, $(OBJ_SO))
	$(GCC) $(CFLAGS) $(SO_FLAGS) -o $@ $^

$(BIN_OBJ)/%.o : %.c
	$(GCC) $(CFLAGS) -c -o $@ $^ 


lint-check : *.h *.cpp *.c
	splint +posixlib +D__gnuc_va_list=int -fileextensions $^ 

clean:
	rm -rf ${BIN_OBJ}
	rm -rf ${BINDIR}
	rm -rf ${SO_OBJ}
	rm -rf ${OUTLIBDIR}
	rm -rf ${OUTINCDIR}
	rm -rf ../include
	rm -f tags cscope.*

$(TARGET_TEST_BIN) : $(BINDIR)/%-test: $(addprefix $(BIN_OBJ)/, %-test.o) $(TARGET_SO) $(TARGET_INC)
	$(GCC) $(CFLAGS) -o $@ $< $(LDFLAGS) -L$(OUTLIBDIR) -lstutils 

test: preparedir $(TARGET_TEST_BIN)
	@result=0; for x in $(TARGET_TEST_BIN); do printf "Running $$x ..."; ./$$x >/dev/null 2>&1; if [ $$? -ne 0 ]; then echo "... FAIL $$x"; result=1; else echo "... SUCCESS";  fi;  done; exit $$result

