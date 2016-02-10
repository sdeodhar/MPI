#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>
#include <string.h>
#include "functions.h"

void global_sum(double *result, int my_rank, int my_size, double my_value)
{
	int i,x = my_size;
	double rcv_value = 0;
	int send_task,power = 0;
	int my_size_org = my_size;
	while (((x % 2) == 0) && x > 1) 
   	{	
		x /= 2;	
		power++;
	}

	//printf("In task %d global_sum My_val is %f power is %d\n",my_rank,my_value,power);
	*result = my_value;
	//int cur_size = my_size;
	
	for (i = 0  ;i<power && my_size!=0; i++)
	{	
		if( (my_rank >= (my_size/2)) && (my_rank < my_size))
		{
			send_task = (my_size - my_rank) - 1;
			printf("Task %d sends to Task %d\n",my_rank,send_task);

			MPI_Send(&my_value, 1, MPI_DOUBLE, ((my_size - my_rank) -1) , 1, MPI_COMM_WORLD);
			//printf("Task %d has myval = %f\n",my_rank,my_value);
		}
		else if(my_rank < (my_size/2))
		{
			MPI_Recv(&rcv_value,1, MPI_DOUBLE, MPI_ANY_SOURCE , 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("Receive Task is %d\n",my_rank);
			*result = *result + rcv_value ;
			my_value = *result;
			//printf("Task %d gets rcvval is %f\n",my_rank,rcv_value);
		}

		my_size = my_size/ 2;
	}
	
		
	if( my_rank == 0)
	{	
		for( i = 1 ; i < my_size_org; i++)
			MPI_Send(&my_value, 1, MPI_DOUBLE, i , 1, MPI_COMM_WORLD);
	} 	
	else
	{		MPI_Recv(&rcv_value,1, MPI_DOUBLE, MPI_ANY_SOURCE , 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			*result = rcv_value;
	}
}


double distributor_func(int cur_rank,int cur_size,char* ofilep)
{
	int i , j;
	double dist,distributer_partial_sum =0,num;
	FILE *fpp;
	fpp=fopen( ofilep, "rb");
	if (!fpp)
	{
		printf("Parallel main Unable to open file!\n");
		return -1;
	}
	
	
	fread(&num,sizeof(num),1,fpp);
 	printf("Total num = %f\n",num);
	
	for( i = 0 ; i < num ; i=i)
	{
		//printf(" i = %d\n",i);

		for( j = 0 ; j < cur_size; j++)
		{
			i++;
			if( i > num ) break;
			fread(&dist,sizeof(dist),1,fpp);
			//printf(" dist = %f\n",dist);
			if( j != cur_rank)
			{
				MPI_Send(&dist, 1, MPI_DOUBLE, j , 1, MPI_COMM_WORLD);
			}
			else
			{
				distributer_partial_sum = distributer_partial_sum + dist;	// Calculating partial sum for task doing the heavy lifting of distribution
												// A case statement may be used here to incorporate different functions for future use
												// Also the work of finding partial sum may be done in main by returning an array
												// but would just add to more work
			}
		}
	}
		
	dist = 101;
	for( j = 0 ; j < cur_size ; j++)
	{
		if( j != cur_rank ) MPI_Send(&dist, 1, MPI_DOUBLE, j , 1, MPI_COMM_WORLD);
	}

	fclose(fpp);
	return distributer_partial_sum;
	
}

