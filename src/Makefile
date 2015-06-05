UNAME_S := $(shell uname -s)

GCC = gcc
CFLAGS = -Wall -pipe -g -m64
CFLAGS += -march=native -mtune=native -O3
CFLAGS += -DNDEBUG
#CFLAGS += -pg
#LDFLAGS += -pg

LDFLAGS += -lpthread

OUTLIBDIR = ../lib
OUTINCDIR = ../include/stutils
SO_OBJ = ./so_obj

TARGET_INC = st_dict.h st_alphabet.h st_utils.h st_conf.h st_log.h \
             st_macro.h st_queue.h st_stack.h st_heap.h st_net.h \
             st_io.h st_opt.h

OBJ_SO = st_dict.o st_alphabet.o st_utils.o st_conf.o st_log.o \
         st_queue.o st_stack.o st_heap.o st_net.o st_io.o st_opt.o

ifeq ($(UNAME_S),Darwin)
TARGET_SO = libstutils.dylib
SO_FLAGS = -dynamiclib
else
TARGET_SO = libstutils.so
SO_FLAGS = -shared
endif

all:  preparedir $(TARGET_SO) 

preparedir: 
	rm -rf $(SO_OBJ)
	mkdir -p $(SO_OBJ)
	rm -rf ${OUTLIBDIR}
	mkdir -p ${OUTLIBDIR}
	rm -rf ${OUTINCDIR}
	mkdir -p ${OUTINCDIR}

$(SO_OBJ)/%.o : %.c 
	$(GCC) $(CFLAGS) -fPIC -c -o $@ $<

$(TARGET_SO) : $(addprefix $(SO_OBJ)/, $(OBJ_SO))
	$(GCC) $(CFLAGS) $(SO_FLAGS) -o $@ $^ $(LDFLAGS)
	mv ${TARGET_SO} ${OUTLIBDIR}
	cp ${TARGET_INC} ${OUTINCDIR}


lint-check : *.h *.cpp *.c
	splint +posixlib +D__gnuc_va_list=int -fileextensions $^ 

clean:
	rm -rf ${SO_OBJ}
	rm -rf ${OUTLIBDIR}
	rm -rf ${OUTINCDIR}
	rm -rf ../include
	rm -f tags cscope.*

