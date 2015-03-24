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

TARGET_INC = st_dict.h st_alphabet.h st_utils.h stconf.h stlog.h \
             st_macro.h st_queue.h st_stack.h st_heap.h st_net.h st_io.h

OBJ_SO = st_dict.o st_alphabet.o st_utils.o stconf.o stlog.o \
         st_queue.o st_stack.o st_heap.o st_net.o st_io.o

TARGET_SO = libstutils.so

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
	$(GCC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)
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

