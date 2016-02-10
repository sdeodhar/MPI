
/**************************************

Name: Print-graph -- program to print data in matrix graph format

Author: Shreya, Deodhar

Last Modified: 10/10/2015

Description: Reads the input file provided.
First entry read : n number of nodes.
And prints in ascii the data read from the file in an n X n matrix format


*******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int num,cur_num,sum,edge_no;
	int i,opt;
	char *ofile;
	const char defarg[22] = "default-graph-file.dat";
	
	sum = 0;
	while((opt = getopt(argc, argv, "i:")) != -1)
	{
		switch(opt)
		{

			case 'i':
				ofile = strdup(optarg);
				break;

			case '?':
				if(optopt == 'i') ofile = strdup(defarg);
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
	edge_no = num * num;
	//printf("Number of N.O.s in file = %f\n",num);	
	printf("\nMatrix in file is %d X %d: \n\n		",num,num);
	for(i = 1; i <= num; i++)
	{
		printf("%03d  ",i);
	}
	printf("\n	- - - - - - - - - - - - - - - \n	001|	");

	for(i = 1; i < (edge_no+1); i++)
	{
		fread(&cur_num,sizeof(cur_num),1,fp);
		if(cur_num >= 202) { cur_num = -1;}					// INF
		printf("%03d  ",cur_num);
		if( !(i%num)){if(((i/num)+1) <= num)	printf("\n	%03d|	",((i/num)+1));	}
	}
	
	printf("\n");	
	fclose(fp);
	return 0;
}
