all:	deodhar_shreya_hw10fin
deodhar_shreya_hw10fin:	deodhar_shreya_hw10fin.o
	mpicc deodhar_shreya_hw10fin.o -o deodhar_shreya_hw10fin

deodhar_shreya_hw10fin.o:	deodhar_shreya_hw10fin.c
	mpicc -c  deodhar_shreya_hw10fin.c -o  deodhar_shreya_hw10fin.o   
clean:
	rm -f deodhar_shreya_hw10fin.o deodhar_shreya_hw10fin
