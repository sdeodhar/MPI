#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#include <getopt.h>
#include <string.h>
#include "MyMPI.h"
#include "functions.h"

int main(int argc, char *argv[])
{
        int rank, size,x;
	double Sum =0;
	double partial_sum=0,mynum=0;
	int i = 0;
	int num = 100, opt;

	char *ofile;
	const char defarg[22] = "default-list-file.dat";
	
	while((opt = getopt(argc, argv, "i:")) != -1)
	{
		switch(opt)
		{
			case 'i':
				ofile = strdup(optarg);
				break;

			case '?':
				if(optopt == 'i') ofile = strdup(defarg);
				printf("No input file defined\nTaking Default file\n");
				break;

			default: printf("Wrong Keyword\n");
				break;
		}
	}
	

	MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	//check to see if size is power of 2
	
	x = size;
	while (((x % 2) == 0) && x > 1) 
   	x /= 2;	
	if(x==1){}
	else{	printf("Error : size is not apower of two : exiting\n"); return 0;}
	
	if(rank == 0)
	{
		//printf("Task %d distributing\n",rank);
		
		partial_sum = distributor_func(rank,size,ofile);	//distributor reads file and distributes number to be added
		
		//printf("Task %d partial sum = %f\n",rank,partial_sum);

	}

	else
	{
		//printf("Task %d waiting for dist\n",rank);
		
		for( i = 0 ; i < num; i++ )
		{	
			//printf("receive loop i = %d , num = %d\n",i,num);	//used in debugging

			MPI_Recv(&mynum, 8, MPI_DOUBLE, 0 , 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	// receiving from distributor

			//printf(" mynum = %f\n",mynum);

			if(mynum == 101) break;								// As max limit on data while generating is 100, 101 was passes to check for exitting
													// To be more cautious, an unsigned int may be used and a negative value may be passed
			partial_sum = partial_sum + mynum;			

			//printf("Task %d intermediate partial sum = %f\n",rank,partial_sum);		//for debugging
			
		}
		//printf("Task %d partial sum = %f\n",rank,partial_sum);		
	}
	
	//printf("\nTask %d starting global sum\n",rank);
	global_sum(&Sum, rank, size, partial_sum);							//global sum calculated by every task 
	
	printf("Task %d  has Final Global Sum = %f \n", rank,Sum);
	MPI_Finalize();
        return 0;
}
