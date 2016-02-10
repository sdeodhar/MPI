
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "graph.h"

inline int min(int x, int y) {
//printf("\n in min\n");
  return x < y ? x : y;
}

int get_rank_type(int r,int cur_size,int n)
{
	if( r == (cur_size - 1)) return CORNER_TASK;
	else if( r >= (cur_size-n)) return BOTTOM_BORDER_TASK;
	else if( ((r+1) % n) == 0 ) return SIDE_BORDER_TASK;
	else return 0;
}

int sqroot(int elt)
{
//printf("\nin sqroot\n");
    int low,high,mid;
    low=1;
    high=elt;
    while(low<=high)
    {
        mid = (low+high)>>1;
        if(mid*mid == elt) return mid;
        if(mid*mid > elt)
            high = mid -1 ;
        else
            low = mid + 1;
    }
    return high;	
}

int read_graph(char *file_name, int n, int **A)
{
	int i,j,num;
	FILE *fp;
	fp=fopen( file_name, "rb+");
	if (!fp)
	{
		printf("Unable to open file!");
		return -1;
	}
	fread(&num,sizeof(num),1,fp);
	if(num != n) printf("\nFile curropted\n");

	printf("\nnum is = %d \n",num);

	for(i = 0; i < num; i++) 
	{
		A[i] = (int *)malloc(num * sizeof(int));
		for(j = 0; j < num; j++) 
		{
			fread(&A[i][j],sizeof(A[i][j]),1,fp);//(fp, "%d ", &A[i][j]);
			if(A[i][j] == -1) A[i][j] = INF;
		}
	}
	printf("\nnum is = %d \n",num);
	fclose(fp);
	return 0;	
}

void write_graph (char *file_name, int n, int **A)
{
	int i,j;
	FILE *fp;
	fp = fopen( file_name, "wb+");//("sp", "w");
	printf("\nnum is = %d \n",n);
	fwrite(&n, sizeof(n), 1, fp);
	for(i = 0; i < n; i++) 
	{
		for(j = 0; j < n; j++) 
		{
			fwrite(&A[i][j], sizeof(A[i][j]), 1, fp);	//fwrite(fp, A[i][j]);
		}
		//fprintf(fp, "\n");
	}
	printf("\nnum is = %d \n",n);
	fclose(fp);
}

int print_graph(int n, char* filename)
{
	int i,edge_no,cur_num;
	FILE *fp;
	fp=fopen( filename, "rb+");
	if (!fp)
	{
		printf("Unable to open file!");
		return -1;
	}
	
	fread(&cur_num,sizeof(cur_num),1,fp);
	if(cur_num != n) printf("\nFile curropted\n");
	edge_no = n * n;

	printf("\nMatrix in file is %d X %d: \n\n		",n,n);

	for(i = 1; i <= n; i++)
	{
		printf("%03d  ",i);
	}
	printf("\n	- - - - - - - - - - - - - - - \n	001|	");

	for(i = 1; i < (edge_no+1); i++)
	{
		fread(&cur_num,sizeof(cur_num),1,fp);
		printf("%03d  ",cur_num);
		if( !(i%n)){if(((i/n)+1) <= n)	printf("\n	%03d|	",((i/n)+1));	}
	}
	
	printf("\n");
	fclose(fp);
	return 0;
}


