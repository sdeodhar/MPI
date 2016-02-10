all:	
	gcc -o floyd-serial floyd-serial.c 
	gcc -o print-graph print-graph.c 
	gcc -o make-graph make-graph.c
	mpicc -g -Wall -Wstrict-prototypes -o floyd-parallel ./floyd-parallel.c graph.c

clean:
	rm -f make-graph print-graph floyd-serial floyd-parallel
