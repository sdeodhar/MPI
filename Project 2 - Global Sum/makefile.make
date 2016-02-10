CFLAGS = -g -Wall -Wstrict-prototypes
PROGS = runmain
OBJECTS = parallel-add-list.o functions.o MyMPI.o
LDFLAGS = -lm
CC = gcc
MCC = mpicc

all:	$(PROGS)

runmain:	$(OBJECTS)
	$(MCC) $(LDFLAGS) -o runmain $(OBJECTS)

parallel-add-list.o:	parallel-add-list.c
	$(MCC) $(CFLAGS) -c parallel-add-list.c

functions.o:	functions.c	functions.h
	$(MCC) $(CFLAGS) -c functions.c

MyMPI.o:	MyMPI.c	MyMPI.h
	$(MCC) $(CFLAGS) -c MyMPI.c

clean:
	rm -f $(PROGS) *.o core*
