all:	
	gcc -o roommate-serial roommate-serial.c 
	mpicc -g -Wall -Wstrict-prototypes -o roommate-parallel ./roommate-parallel.c

clean:
	rm -f roommate-serial roommate-parallel

