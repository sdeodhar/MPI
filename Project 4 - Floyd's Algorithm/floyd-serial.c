/**************************************
Name : Floyd-serial -- program to execute the Floyd's shortest path algorithm on the input graph file.

Author: Shreya, Deodhar

Last Modified: 10/10/2015

Description: 
Reads a graph file, 
computes Floyd's shortest pairs in a serial manner, 
writes resulting shortest path graph, to be stored in the same format
*******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include "graph.h"

//#define INF 202

inline int min(int a, int b) 
{
  return a < b ? a : b;
}

int sequential_floyd(int n, int **A) 
{
	int i, j, k;
	for(k = 0; k < n; k++) 
	{
		for(i = 0; i < n; i++) 
		{
			for(j = 0; j< n; j++) 
			{
				//if(i != j)  
				{
					//if(A[i][j] == -1) A[i][j] = INF;

					A[i][j] = min(A[i][j], A[i][k] + A[k][j]);
				}
			}
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int num,cur_num,sum,edge_no;
	int i,j,opt;
	char *ofile,*outfile;

	const char defarg[22] = "default-graph-file.dat";
	const char defout[22] = "default-graph-outf.dat";
	
	sum = 0;
	clock_t pbegin, pend ,fl_begin , fl_end;
	double time_spent,floyd_time_spent;
        pbegin = clock();

	while((opt = getopt(argc, argv, "i:o:")) != -1)
	{
		switch(opt)
		{
			case 'i':
				ofile = strdup(optarg);
				break;
			case 'o':
				outfile = strdup(optarg);
				break;

			case '?':
				if(optopt == 'i') ofile = strdup(defarg);
				if(optopt == 'o') outfile = strdup(defout);
				break;

			default: printf("Wrong Keyword\n");
				break;

		}

	}
		
	FILE *fp;
	fp=fopen( ofile, "rb+");
	if (!fp)
	{
		printf("Unable to open file!");
		return -1;
	}
	fread(&num,sizeof(num),1,fp);
	printf("\nnum is = %d \n",num);	
	
	int **A = (int **)malloc(num * sizeof(int*));
	for(i = 0; i < num; i++) 
	{
		A[i] = (int *)malloc(num * sizeof(int));
		for(j = 0; j < num; j++) 
		{
			fread(&A[i][j],sizeof(A[i][j]),1,fp);//(fp, "%d ", &A[i][j]);
			if(A[i][j] == -1) A[i][j] = INF;
		}
	}
//	printf("\nnum is = %d \n",num);
	fclose(fp);
	fl_begin = clock();
	
	sequential_floyd(num, A);
	fl_end = clock();
	floyd_time_spent = (double)(fl_end - fl_begin) / CLOCKS_PER_SEC;
	printf("\nFloyd algo time = %f\n",floyd_time_spent);

//	printf("\nnum is = %d \n",num);
	fp = fopen( outfile, "wb+");//("sp", "w");
//	printf("\nnum is = %d \n",num);
	fwrite(&num, sizeof(num), 1, fp);
	for(i = 0; i < num; i++) 
	{
		for(j = 0; j < num; j++) 
		{
			fwrite(&A[i][j], sizeof(A[i][j]), 1, fp);	//fwrite(fp, A[i][j]);
		}
		//fprintf(fp, "\n");
	}
//	printf("\nnum is = %d \n",num);
	fclose(fp);
	pend = clock();
	time_spent = (double)(pend - pbegin) / CLOCKS_PER_SEC;
	printf("\nTotal Execution time = %f\n",time_spent);


}
