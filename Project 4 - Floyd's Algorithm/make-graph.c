/**************************************

Name: Make-graph -- program to generate a graph

Author: Shreya, Deodhar

Last Modified: 10/10/2015

Description: Generates data file with 
n nodes
p probability of edge weight being infinite
r range of edge weight

Writes to an output file which is a binary file with an n by n matrix representing a graph of n nodes, with directional edge weights 0 to r and a probability of an edge of r/p. 
Non-edges are represented with -1. 
Data are integers and the number n is stored as the first word of the file.

*******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int num,range,prob,cur_num,edge_no;
	int i,j,opt,seedi = 50;
	char *ofile;
	const char defarg[22] = "default-graph-file.dat";

	while((opt = getopt(argc, argv, "n:r:p:o:")) != -1)
	{
		switch(opt)
		{
			case 'n':
				num = atoi(optarg);
				break;

			case 'r':
				range = atoi(optarg);
				break;

			case 'p':
				prob = atoi(optarg);
				break;

			case 'o':
				ofile = strdup(optarg);
				break;

			case '?':
				if(optopt == 'n') num = 100;
				if(optopt == 'r') range = 100;
				if(optopt == 'p') prob = 150;
				if(optopt == 'o') ofile = strdup(defarg);
				break;

			default: printf("Wrong Keyword\n");
				break;

		}

	}
		
	FILE *fp;
	fp=fopen( ofile, "wb+");
	if (!fp)
	{
		printf("Unable to open file!");
		return -1;
	}
	
	cur_num = num;
	fwrite(&cur_num, sizeof(cur_num), 1, fp);	
	srandom(seedi);
	edge_no = num * num;

	//for(i = 0; i < edge_no; i++)
	for(i = 0; i < num; i++)
	{
		for( j = 0 ; j < num ; j++)
		{
			if(i == j) cur_num = 0;
			else 
			{
				cur_num = random() % prob;				
				if(cur_num <= range) {}
				else cur_num = -1;
			}

//		printf("cur edge weight is %d\n",cur_num);
		  fwrite(&cur_num, sizeof(cur_num), 1, fp);
		}	
	}
	printf("writting graph to file\n");
	fclose(fp);
	return 0;
}
