CC=gcc
CFLAGS=-Wall -g 
EXEC=esasynch esasynch2 remontee recopie synchro remontee_str barriere
TAR=MedinaRoberto.mi014.9

all: $(EXEC)

esasynch: obj/esasynch.o
	$(CC) -o bin/esasynch obj/esasynch.o -lrt

obj/esasynch.o: src/esasynch.c
	$(CC) -o obj/esasynch.o -c src/esasynch.c $(CFLAGS)

esasynch2: obj/esasynch2.o
	$(CC) -o bin/esasynch2 obj/esasynch2.o -lrt

obj/esasynch2.o: src/esasynch2.c
	$(CC) -o obj/esasynch2.o -c src/esasynch2.c $(CFLAGS)

remontee: obj/remontee.o
	$(CC) -o bin/remontee obj/remontee.o -lrt

obj/remontee.o: src/remontee.c
	$(CC) -o obj/remontee.o -c src/remontee.c $(CFLAGS)

recopie: obj/recopie.o
	$(CC) -o bin/recopie obj/recopie.o -lrt

obj/recopie.o: src/recopie.c
	$(CC) -o obj/recopie.o -c src/recopie.c $(CFLAGS)

synchro: obj/synchro.o
	$(CC) -o bin/synchro obj/synchro.o -lrt

obj/synchro.o: src/synchro.c
	$(CC) -o obj/synchro.o -c src/synchro.c $(CFLAGS)

remontee_str: obj/remontee_str.o
	$(CC) -o bin/remontee_str obj/remontee_str.o -lrt

obj/remontee_str.o: src/remontee_str.c
	$(CC) -o obj/remontee_str.o -c src/remontee_str.c $(CFLAGS)

barriere: obj/barriere.o
	$(CC) -o bin/barriere obj/barriere.o -lrt

obj/barriere.o: src/barriere.c
	$(CC) -o obj/barriere.o -c src/barriere.c $(CFLAGS)

tar:
	tar -cvf - ../$(TAR) | gzip >../$(TAR).tgz

clean:
	rm obj/*.o bin/*

package: clean tar
