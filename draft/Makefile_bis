LIB = lib
PAPI_LIB = lib/libpapi32.a
CFLAGS = -Wall -g -D_GNU_SOURCE

CC=gcc $(CFLAGS)

BIN=bin
SRC=src
OBJ=obj
INC=include

EXEC=papi_wrapper
TAR = PSAR

all: $(BIN)/$(EXEC)

$(BIN)/$(EXEC) : $(OBJ)/$(EXEC).o
	$(CC) $< $(PAPI_LIB) -o $@

$(OBJ)/$(EXEC).o : $(SRC)/$(EXEC).c
	$(CC) -I $(INC) -c $< -o $@

tar:
	tar -cvf - ../$(TAR) | gzip >../$(TAR).tgz

clean:
	rm obj/*.o bin/*

tests:
	make -f Makefile_tests

package: clean tar
