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
TEST_SRC=chash-test.c
TEST_OBJ=chash-test.o
TEST_EXE=chash-test

CFLAGS += -Werror -Wall -Wextra -Werror=cast-qual -pedantic -fomit-frame-pointer -pipe
LDFLAGS += -L.
LDADD += -lchash
CC=gcc

ifeq ("$(PREFIX)", "")
PREFIX=/usr/local
endif

ifeq ("$(LIBDIR)", "")
LIBDIR=$(PREFIX)/lib
endif

ifeq ("$(INCDIR)", "")
INCDIR=$(PREFIX)/include
endif

ifneq ($(strip $(srcdir)),)
   VPATH::=$(srcdir)
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

$(TEST_EXE): library
	$(CC) -c $(INCLUDES) $(CFLAGS) $(TEST_SRC) -o $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(A_NAME) -o $(TEST_EXE)-static
	$(CC) $(TEST_OBJ) $(LDFLAGS) -o $(TEST_EXE)-dynamic $(LDADD)

check: $(TEST_EXE)
	./$(TEST_EXE)-static
	LD_LIBRARY_PATH=. ./$(TEST_EXE)-dynamic

valgrind: $(TEST_EXE)
	valgrind ./$(TEST_EXE)-static

tidy:
	indent -npro -kr -i4 -sob -l80 -ss -cs -cp1 \
		-T size_t \
		*.h *.c

clean:
	rm -f *.o *.a *.$(SHAREDEXT)  $(SO_NAME).* \
		$(TEST_EXE)-static $(TEST_EXE)-dynamic

install: library
	 @echo "Installing libraries in $(LIBDIR)"
	 @mkdir -pv $(LIBDIR)/
	 @cp -pv $(A_NAME) $(LIBDIR)/
	 @cp -Rv $(SO_NAME)* $(LIBDIR)/
	 @echo "Installing headers in $(INCDIR)"
	 @mkdir -pv $(INCDIR)/
	 @cp -pv $(LIB_HDR) $(INCDIR)/
