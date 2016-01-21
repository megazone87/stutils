CFLAGS = -Wall -pipe -g -m64
CFLAGS += -march=native -mtune=native -O3
CFLAGS += -I. -I$(OUTINC_DIR)
CFLAGS += -DNDEBUG
#CFLAGS += -pg
#LDFLAGS += -pg
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDFLAGS+=
else
LDFLAGS+=-lm
endif
LDFLAGS += -lpthread
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
LDFLAGS += -Wl,-rpath,$(shell pwd)/$(OUTLIB_DIR)
