all:	shreya_deodhar_hw02
shreya_deodhar_hw02:	shreya_deodhar_hw02.o
	mpicc shreya_deodhar_hw02.o -o shreya_deodhar_hw02

shreya_deodhar_hw02.o:	shreya_deodhar_A2_hw02.c
	mpicc -c  shreya_deodhar_A2_ring.c -o  shreya_deodhar_hw02.o   
clean:
	rm -f shreya_deodhar_hw02.o shreya_deodhar_hw02
