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

all: bin/papi_hypervisor bin/papi_wrapper bin/attack_task bin/rt_task bin/attack_task2

bin/papi_notifier: obj/papi_notifier.o obj/papi_util.o
	$(CC) $(CFLAGS) -o $@ $^ $(PAPI_LIB) -lrt

bin/papi_hypervisor: obj/papi_hypervisor.o obj/papi_util.o
	$(CC) $(CFLAGS) -o $@ $^ $(PAPI_LIB) -lrt

bin/papi_wrapper: obj/papi_wrapper.o obj/papi_util.o
	$(CC) $(CFLAGS) -o $@ $^ $(PAPI_LIB)

bin/rt_task: obj/rt_task.o
	$(CC) -o $@ $^

bin/attack_task: obj/attack_task.o obj/papi_util.o
	$(CC) -o $@ $^ $(PAPI_LIB)

bin/attack_task2: obj/attack_task2.o obj/papi_util.o
	$(CC) -o $@ $^ $(PAPI_LIB)

obj/papi_hypervisor.o: src/papi_hypervisor.c $(INC)/papi_util.h
	$(CC) $(CFLAGS) -I $(INC) -c $< -o $@

obj/papi_util.o: src/papi_util.c
	$(CC) -I $(INC) -c $< -o $@

obj/papi_wrapper.o: src/papi_wrapper.c $(INC)/papi_util.h
	$(CC) $(CFLAGS) -I $(INC) -c $< -o $@

obj/rt_task.o: src/rt_task.c
	$(CC) -c -o $@ $^

obj/attack_task.o: src/attack_task.c include/papi_util.h
	$(CC)  $(CFLAGS) -I $(INC) -c $< -o $@

obj/attack_task2.o: src/attack_task2.c include/papi_util.h
	$(CC)  $(CFLAGS) -I $(INC) -c $< -o $@

tar:
	tar -cvf - ../$(TAR) | gzip >../$(TAR).tgz

clean: 
	rm obj/* bin/*

package: clean tar
