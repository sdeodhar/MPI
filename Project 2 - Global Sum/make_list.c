#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[])
{
	double num,cur_num;
	int i,opt,seedi = 3;
	char *ofile;
	const char defarg[22] = "default-list-file.dat";

	while((opt = getopt(argc, argv, "n:o:")) != -1)
	{
		switch(opt)
		{
			case 'n':
				num = atoi(optarg);
				break;

			case 'o':
				ofile = strdup(optarg);
				break;

			case '?':
				if(optopt == 'n') num = 100;
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

	for(i = 0; i < num; i++)
	{
		cur_num = (double)( random() % 100 );
		cur_num *= 0.25;
		printf("curnum is %f\n",cur_num);
		fwrite(&cur_num, sizeof(cur_num), 1, fp);	
	}
	printf("List generated\n");
	fclose(fp);
	return 0;
}
