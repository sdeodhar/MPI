#include <stdio.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int N,num,cur_num,sum,edge_no;
	int i,j,z,total_dislike,opt;
	double T;
	int cur_assignment[N];
	char *ofile;
	//const char defarg[22] = "default-graph-file.dat";
	
	sum = 0;
	while((opt = getopt(argc, argv, "i:")) != -1)
	{
		switch(opt)
		{

			case 'i':
				ofile = strdup(optarg);
				break;

			case '?':
				//if(optopt == 'i') ofile = strdup(defarg);
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
	
	printf("Number of candidates in file = %d\n",num);	
	
	for(j = 0; j < 100000 ; j++)
	{
		fread(&i,	       sizeof(i), 	      1, fp);
		printf("\nIteration = %d\t",i);
		fread(&T,	       sizeof(T), 	      1, fp);
		printf("Temperature = %f\t",T);
		fread(&total_dislike, sizeof(total_dislike), 1, fp);
		printf("Total dislike = %d\n",total_dislike);
		printf("Current Assignment = %d\t");
		for (z=0; z < N; z++) {fread(&cur_assignment[z], sizeof(cur_assignment[z]), 1, fp);printf(" %d ",cur_assignment[z]);}
		
	}
	
	printf("\n");	
	fclose(fp);
	return 0;
}
