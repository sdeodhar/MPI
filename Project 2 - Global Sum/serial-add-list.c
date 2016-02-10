#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

int main(int argc, char *argv[])
{
	double num,cur_num,sum;
	int i,opt;
	char *ofile;
	const char defarg[22] = "default-list-file.dat";
	
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
	//printf("num = %f\n",num);	

	for(i = 0; i < num; i++)
	{
		fread(&cur_num,sizeof(cur_num),1,fp);
		//printf("%f\n",cur_num);
		sum = sum + cur_num;
	}
	
	printf("sum = %f\n",sum);
	fclose(fp);
	return 0;
}
