INCLUDES = -I../include
LIBS = -L../lib
CC = gcc

siggen: siggen.c wave.c ../lib/libportsf.a
	$(CC) -o siggen $(INCLUDES) $(LIBS) siggen.c wave.c -lm -lportsf

../lib/libportsf.a:	
	cd ../portsf; make; make install
