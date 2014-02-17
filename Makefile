CC=gcc
INCL=-I/usr/local/include
PATH=/usr/local/lib/libpapi.a
CFLAGS=-Wall -g 
TAR=PSAR
EXEC=papi_wrapper

all: $(EXEC)

papi_wrapper: src/papi_wrapper.c
	$(CC) $(INCL) obj/papi_wrapper.c $(PATH) -o bin/papi_wrapper 

obj/papi_wrapper.o: src/papi_wrapper.c
	$(CC) $(INCL) -O0 src/papi_wrapper.c $(CFLAGS) -o obj/papi_wrapper.o

tar:
	tar -cvf - ../$(TAR) | gzip >../$(TAR).tgz

clean:
	rm obj/*.o bin/*

package: clean tar
