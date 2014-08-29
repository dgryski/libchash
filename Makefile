UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

LIB_SRC=chash.c
LIB_HDR=chash.h
LIB_OBJ=$(LIB_SRC:.c=.o)
SO_OBJS=chash.o
SO_NAME=libchash.$(SHAREDEXT)
ifneq ($(UNAME), Darwin)
    SHAREDFLAGS += -Wl,-soname,$(SO_NAME)
endif

A_NAME=libchash.a

INCLUDES=-I.
SRC=chash-test.c
OBJ=chash-test.o
OUT=chash-test

CFLAGS += -Werror -Wall -Wextra -pedantic
LDFLAGS += -L. -lchash
CC=gcc

ifeq ("$(LIBDIR)", "")
LIBDIR=/usr/local/lib
endif

ifeq ("$(INCDIR)", "")
INCDIR=/usr/local/include
endif

default: library

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $< -o $@

$(SO_NAME): $(LIB_OBJ)
	$(CC) $(SHAREDFLAGS) -o $(SO_NAME).1.0 $(SO_OBJS)
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME).1
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME)

$(A_NAME): $(LIB_OBJ)
	ar -r $(A_NAME) $(SO_OBJS)

library: $(SO_NAME) $(A_NAME)

$(OUT): library
	$(CC) -c $(INCLUDES) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CC) $(OBJ) $(A_NAME) -o $(OUT)-static
	$(CC) $(OBJ) $(LDFLAGS) -o $(OUT)-dynamic

check: $(OUT)
	./$(OUT)-static
	LD_LIBRARY_PATH=. ./$(OUT)-dynamic

clean:
	rm -f *.o *.a *.$(SHAREDEXT)  $(SO_NAME).* $(OUT)-static $(OUT)-dynamic

install:
	 @echo "Installing libraries in $(LIBDIR)"; \
	 cp -pv $(A_NAME) $(LIBDIR)/;\
	 cp -Rv $(SO_NAME)* $(LIBDIR)/;\
	 echo "Installing headers in $(INCDIR)"; \
	 cp -pv $(LIB_HDR) $(INCDIR)/;
