/**************************************

Floyd-parallel -- program to execute the Floyd's shortest 
path algorithm on the input graph file in a parallel manner.

Author: Shreya, Deodhar

Last Modified: 10/21/2015

Description:
Computes Floyd's shortest pairs in parallel 
manner and writes resulting shortest path graph. 
2D checkerboard decomposition is used to distribute 
data among the parallel tasks.

*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include "graph.h"

/****************Floyd's Algorithm in parallel***************/

int floyd_par(int num, int localnum, int *A, MPI_Comm comm_2d, MPI_Comm comm_row, MPI_Comm comm_col) 
{
	int i, j, k;
	int my2drank, mycoords[2];
	int row_root, col_root, coords[2];

	MPI_Comm_rank(comm_2d, &my2drank);
	MPI_Cart_coords(comm_2d, my2drank, 2, mycoords);

	int *myrow = (int *)malloc(localnum * sizeof(int));
	if(myrow == NULL) { printf("\nMEM ALLOCATION FAILED.....!!!!!\n");}

	int *mycol = (int *)malloc(localnum * sizeof(int));
	if(mycol == NULL) { printf("\nMEM ALLOCATION FAILED.....!!!!!\n");}

	for(k = 0; k < num; k++) 
	{
		coords[ROW] = k / localnum;
		coords[COL] = k / localnum;

		if(k >= mycoords[ROW] * localnum && k <= (mycoords[ROW] + 1) * localnum) 
		{
			for(i = 0; i < localnum; i++) 
			{
				myrow[i] = A[(k % localnum)*localnum + i];			// Get the row values to be used in k iteration
				/*if(myrow[i]==-1) 
				{
					myrow[i] = INF;						//  if row val if found to be -1, set it to a maximum value more than twice the range of the read graph
					//A[(k % localnum)*localnum + i] = INF;
				}*/
			}
		}

		MPI_Cart_rank(comm_col, coords, &col_root);

		MPI_Barrier(comm_col);								// wait for all tasks in comm_col to complete till here for synchronization
		MPI_Bcast(&myrow[0], localnum, MPI_INT, col_root, comm_col);			// Brodcast row values

		if(k >= mycoords[COL] * localnum && k <= (mycoords[COL] + 1) * localnum) 
		{
			for(i = 0; i < localnum; i++) 
			{
				mycol[i] = A[i*localnum + (k % localnum)];			// Get the col values to be used in k iteration
				/*if(mycol[i]==-1) 
				{
					mycol[i] = INF;
					//A[i*localnum + (k % localnum)] = INF;
				}*/
			}
		}

		MPI_Cart_rank(comm_row, coords, &row_root);

		MPI_Barrier(comm_row);								// wait for all tasks in comm_row to complete till here for synchronization
		MPI_Bcast(&mycol[0], localnum, MPI_INT, row_root, comm_row);			// Brodcast col values
		
		//printf("\n");

		for(i = 0; i < localnum; i++) 
		{
			for(j = 0; j < localnum; j++) 
			{

				A[i * localnum + j] = min(A[i * localnum + j], mycol[i] + myrow[j]);	// find shorter path of the two

//printf("k=%d col %d =%d row %d =%d A[%d]=%d\n",k,i,mycol[i],j,myrow[j],i*localnum+j,A[i*localnum+j]);
				
			}
		
		}
	}
	free(myrow);
	free(mycol);
	return 0;
}


int main(int argc, char *argv[]) 
{

	int i, j, n, num_main,opt;	
	MPI_File fp; 
	char* filename;// = "output_file";
	MPI_Comm comm_2D, comm_row, comm_col;
	int num_procs, dims[2], periods[2], dims_to_preserve[2];
	int my_rank, my_2D_rank, my_coords[2];
	MPI_Status status;
	int new_num_main, new_n, cur_rank=0,rowadd=0;

	char *ofilename;
	const char defarg[22] = "default-graph-file.dat";
	const char defoarg[28] = "default-graph-outputfile.dat";
	clock_t begin, end;
	double time_spent;
        begin = clock();
	
	while((opt = getopt(argc, argv, "i:o:")) != -1)
	{
		switch(opt)
		{

			case 'i':
				filename = strdup(optarg);
				break;

			case 'o':
				ofilename = strdup(optarg);
				break;

			case '?':
				if(optopt == 'i') filename = strdup(defarg);
				if(optopt == 'o') ofilename = strdup(defoarg);
				break;

			default: printf("Wrong Keyword\n");
				break;

		}

	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	dims[ROW] = sqroot(num_procs);
	dims[COL] = sqroot(num_procs);

	periods[ROW] = periods[COL] = 1;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &comm_2D);

	MPI_Comm_rank(comm_2D, &my_2D_rank);
	MPI_Cart_coords(comm_2D, my_2D_rank, 2, my_coords);

	dims_to_preserve[ROW] = 0;
	dims_to_preserve[COL] = 1;
	MPI_Cart_sub(comm_2D, dims_to_preserve, &comm_row);

	dims_to_preserve[ROW] = 1;
	dims_to_preserve[COL] = 0;
	MPI_Cart_sub(comm_2D, dims_to_preserve, &comm_col);

	if(MPI_File_open(comm_2D, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fp) != MPI_SUCCESS) 
	{
		fprintf(stderr, "Cannot open file %s\n", filename);
		MPI_Abort(comm_2D, FILE_NOT_FOUND);
		MPI_Finalize();
		return 1;
	}
	MPI_File_seek(fp, 0, MPI_SEEK_SET);
	MPI_File_read(fp, &n, 1, MPI_INT, &status); 			// read number of elements in file
	num_main = n/dims[ROW];

	if( (n % dims[ROW]) != 0 ) 	
	{ 
//		printf("\nElements not distributed properly\n");
	}
	new_num_main = num_main;
	new_n = n;
	//printf("\nBefore new_n= %d   new_num_main = %d\n", new_n , new_num_main);

	while( new_n % dims[ROW] )	{ new_n++; rowadd++; }//printf("\nrowadd = %d\n",rowadd);}		//if the matrix size is not square,
	new_num_main = new_n/dims[ROW];										//add dummy rows and columns with value infinite
														//to perform easy checkerboard decomposition
	
//printf("\nnew_n= %d   new_num_main = %d\n", new_n , new_num_main);

	if(rowadd)	{cur_rank = get_rank_type( my_2D_rank, num_procs, dims[COL]);}				//divide the decomposition blocks into types

//	printf("\nProcess %d cur_rank = %d\n", my_2D_rank,cur_rank);

	int *A = (int *)malloc(new_num_main * new_num_main * sizeof(int));
	if(A == NULL) { printf("\nMEM ALLOCATION FAILED.....!!!!!\n");}

/*************************** Read file and add dummy values as per the type allocated by get_rank_type***************************************************/

	for(i = 0; i < new_num_main; i++) 
	{
	    if( cur_rank == SIDE_BORDER_TASK )
	    {
		MPI_File_seek(fp, ((my_coords[0] * new_num_main  + i) * n + my_coords[1] * new_num_main + 1) * sizeof(int), MPI_SEEK_SET);
		MPI_File_read(fp, &A[i * new_num_main], new_num_main - rowadd, MPI_INT, &status);
		for(j = rowadd; j >= 0 ; j--) A[(i * new_num_main)+j+ new_num_main-rowadd] = -1; 

//		if(i == 0) printf("data read by task %d =\n",my_2D_rank);
		for(j = 0; j < new_num_main; j++) 		
		{
//			printf(" %d ",A[(i * new_num_main)+j]);
			//printf("data read by task %d = %d ",my_2D_rank,A[(i * num_main)+j]);
//			if( j == (new_num_main-1) ) printf("\n");
			if(A[(i * new_num_main)+j] == -1) A[(i * new_num_main)+j] = INF; 
		}

	    }

	    else if( cur_rank == BOTTOM_BORDER_TASK  )
	    {
		if(i < num_main-1) 
		{
			MPI_File_seek(fp,((my_coords[0] *new_num_main+i)*n + my_coords[1] * new_num_main + 1) * sizeof(int), MPI_SEEK_SET);
			MPI_File_read(fp, &A[i * new_num_main], new_num_main, MPI_INT, &status);
		} 
		else
		{
			for(j = 0; j < new_num_main; j++) 		
			{
				A[(i * new_num_main)+j] = -1; 
			}
		}
//		if(i == 0) printf("data read by task %d =\n",my_2D_rank);
		for(j = 0; j < new_num_main; j++) 		
		{
//			printf(" %d ",A[(i * new_num_main)+j]);
			//printf("data read by task %d = %d ",my_2D_rank,A[(i * num_main)+j]);
//			if( j == (new_num_main-1) ) printf("\n");
			if(A[(i * new_num_main)+j] == -1) A[(i * new_num_main)+j] = INF; 
		}

	    }

	    else if( cur_rank == CORNER_TASK)
	    {
		if(i < num_main-1) 
		{
			MPI_File_seek(fp, ((my_coords[0] * new_num_main + i) * n + my_coords[1]*new_num_main+1)* sizeof(int), MPI_SEEK_SET);
			//MPI_File_read(fp, &A[i * new_num_main], num_main, MPI_INT, &status);
			MPI_File_read(fp, &A[i * new_num_main], new_num_main - rowadd, MPI_INT, &status);
			for(j = rowadd; j >= 0 ; j--) A[(i * new_num_main)+j+ new_num_main - rowadd] = -1;
		} 
		else
		{
			for(j = 0; j < new_num_main; j++) 		
			{
				A[(i * new_num_main)+j] = -1; 
			}
		}
//		if(i == 0) printf("data read by task %d =\n",my_2D_rank);
		for(j = 0; j < new_num_main; j++) 		
		{
//			printf(" %d ",A[(i * new_num_main)+j]);
			//printf("data read by task %d = %d ",my_2D_rank,A[(i * num_main)+j]);
//			if( j == (new_num_main-1) ) printf("\n");
			if(A[(i * new_num_main)+j] == -1) A[(i * new_num_main)+j] = INF; 
		}
	
	    }

	    else
	    {

		MPI_File_seek(fp, ((my_coords[0] * new_num_main  + i) * n + my_coords[1] * new_num_main + 1) * sizeof(int), MPI_SEEK_SET);
		MPI_File_read(fp, &A[i * new_num_main], new_num_main, MPI_INT, &status);

//		if(i == 0) printf("data read by task %d =\n",my_2D_rank);
		for(j = 0; j < new_num_main; j++) 		
		{
//			printf(" %d ",A[(i * new_num_main)+j]);
			//printf("data read by task %d = %d ",my_2D_rank,A[(i * new_num_main)+j]);
//			if( j == (new_num_main-1) ) printf("\n");
			if(A[(i * new_num_main)+j] == -1) A[(i * new_num_main)+j] = INF; 
		}

	     }	

/*if(i == 0) printf("data read by task %d =\n",my_2D_rank);
for(j = 0; j < new_num_main; j++) 		
{
	printf(" %d ",A[(i * new_num_main)+j]);
	//printf("data read by task %d = %d ",my_2D_rank,A[(i * num_main)+j]);
	if( j == (new_num_main-1) ) printf("\n");
	if(A[(i * new_num_main)+j] == -1) A[(i * new_num_main)+j] = INF; 
}*/	
	}

	MPI_File_close(&fp);

	double start = MPI_Wtime();
	
	floyd_par(new_n, new_num_main, A, comm_2D, comm_row, comm_col);
	
	double stop = MPI_Wtime();
	printf("Task %d Completed in %2.3f seconds\n", my_2D_rank, stop-start);

	MPI_Comm_free(&comm_col);
	MPI_Comm_free(&comm_row);
//printf("Task %d col & row comm free\n", my_2D_rank);

/********************************** Writing back to the file ignoring the the dummy data added in previous step************************************/

	if(MPI_File_open(comm_2D, ofilename, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fp) != MPI_SUCCESS) 
	{
			printf("Cannot open file %s\n", "out");
			MPI_Abort(comm_2D, FILE_NOT_FOUND);
			MPI_Finalize();
			return 1;
	}

	if(my_2D_rank == 0) 
	{
		MPI_File_seek(fp, 0, MPI_SEEK_SET);
		MPI_File_write(fp, &n, 1, MPI_INT, &status);
	}
	
	for(i = 0; i < new_num_main; i++) 
	{
		MPI_File_seek(fp, ((my_coords[0] * new_num_main  + i) * n + my_coords[1] * new_num_main + 1) * sizeof(int), MPI_SEEK_SET);

		if(cur_rank == 0) 			MPI_File_write(fp, &A[i * new_num_main], new_num_main, MPI_INT, &status);
		else if(cur_rank == SIDE_BORDER_TASK)  	MPI_File_write(fp, &A[i * new_num_main], new_num_main - rowadd, MPI_INT, &status);
		else if(cur_rank == BOTTOM_BORDER_TASK)	{if(i < num_main) MPI_File_write(fp,&A[i*new_num_main],new_num_main,MPI_INT,&status);}
		else if(cur_rank == CORNER_TASK)	MPI_File_write(fp, &A[i * new_num_main], new_num_main - rowadd, MPI_INT, &status);
	}

	MPI_File_close(&fp);
	free(A);

	MPI_Comm_free(&comm_2D);
	MPI_Finalize();
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("\nTotal Execution time = %f\n",time_spent);
	return 0;
}
