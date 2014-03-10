LIB = lib
UNAME := $(shell uname -m)

ifeq ($(UNAME), x86_64)
	PAPI_LIB = lib/libpapi.a
else
	PAPI_LIB = lib/libpapi32.a
endif

CFLAGS = -D_GNU_SOURCE
CC=gcc -Wall -g
INC=include
TAR = PSAR

all: bin/rt_task bin/attaquant_task bin/papi_wrapper bin/attack_task bin/rt_task2

bin/papi_wrapper: obj/papi_wrapper.o
	$(CC) $(CFLAGS) $< $(PAPI_LIB) -o $@

obj/papi_wrapper.o: src/papi_wrapper.c
	$(CC) $(CFLAGS) -I $(INC) -c $< -o $@

bin/rt_task: obj/rt_task.o
	$(CC) -o $@ $^

obj/rt_task.o: src/rt_task.c
	$(CC) -c -o $@ $^	

bin/rt_task2: obj/rt_task2.o
	$(CC) -o $@ $^

obj/rt_task2.o: src/rt_task2.c
	$(CC) -c -o $@ $^

bin/attaquant_task: obj/attaquant_task.o
	$(CC) -o $@ $^

obj/attaquant_task.o: src/attaquant_task.c
	$(CC) -c -o $@ $^

bin/attack_task: obj/attack_task.o
	$(CC) -o $@ $^

obj/attack_task.o: src/attack_task.c
	$(CC) -c -o $@ $^

tar:
	tar -cvf - ../$(TAR) | gzip >../$(TAR).tgz

clean: 
	rm obj/* bin/*

package: clean tar
