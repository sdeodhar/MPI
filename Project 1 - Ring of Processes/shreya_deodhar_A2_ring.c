#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char *argv[])
{
        int rank, size;
        int myNum;
	char count;
	
	int Delta;
	int R_task = 0,L_task = 0;
	int Sum =0;
	char i = 0,tempvar = 0;

	MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);

	srandom(rank);
	myNum = random() % 100;

	printf("Task %d Initial value = %d\n", rank,myNum);
	count = 0;
        tempvar = size;
	
	while( tempvar!=0 )
	{
		tempvar = tempvar>>1;
		count++;
	}
	count--;
		
	printf("SIZE == %d, k == %d, so tasks will execute %d rounds\n",size,count,count);

	Sum = myNum;			//initialize before starting the ring

	int step = 1;			// k = 2 ^ 0 = 1

	for( i = 0; i < count; i++ )
	{
		L_task = (rank + (step<<i));		
		R_task = (rank - (step<<i));
		
		if( L_task >= size ) L_task = L_task - size;
		if( R_task < 0 ) R_task = size + R_task;
		
		//printf("For task %d round %d L_task = %d \n",rank,i,L_task);		//used for debugging
		//printf("For task %d round %d R_task = %d \n",rank,i,R_task);	

		MPI_Send(&Sum  , 1, MPI_INT, L_task , 1, MPI_COMM_WORLD);
		MPI_Recv(&Delta, 1, MPI_INT, R_task , 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		Sum = Sum + Delta;

		printf("Task %d round %d gets Delta = %d from Task %d, Sum = %d \n", rank, i, Delta,R_task, Sum);
        }
	       	
	printf("Task %d  has Final Sum = %d \n", rank,Sum);
	MPI_Finalize();
        return 0;
}
