LIB_SRC=chash.c
LIB_OBJ=$(LIB_SRC:.c=.o)
SO_OBJS=chash.o
SO_NAME=libchash.so

INCLUDES=-I.
SRC=test.c
OBJ=test.o
OUT=test

CFLAGS=-Werror -Wall -Wextra -pedantic
LDFLAGS=-L. -lchash
CC=gcc

default: $(OUT)

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $< -o $@

$(SO_NAME): $(LIB_OBJ)
	$(CC) -shared -Wl,-soname,$(SO_NAME) -o $(SO_NAME).1.0 $(SO_OBJS)
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME).1
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME)

$(OUT): $(SO_NAME)
	$(CC) -c $(INCLUDES) $(CFLAGS) $(SRC) -o $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(OUT)

check: $(OUT)
	LD_LIBRARY_PATH=. ./$(OUT)

clean:
	rm -f *.o *.a *.so  $(SO_NAME).* $(OUT)
