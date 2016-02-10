
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

double random_gen()
{
	int i;
	double pr = 0;
	i = rand() % 1000;
	//printf("raw random %d\n", i);
	pr = i / 1000.0;
	//printf("probability %f\n", pr);
	return pr;
}

int main( int argc, char *argv[])
{
	FILE *ip_matrix;
	int i, j, k, iterations_used, n, N, maxI_no_change, max_iterations = 100000;//start_temp;
	int swap, total_dislike;
	double T, start_temp;
	int new_dislike, roommate1, roommate2, temp, no_change_flg;

	int opt;
        char *dfile, *outfile, *ifile;

	const char defarg[12] = "dislikes.dat";
	const char defobj[16] = "objfuncvalue.dat";
	const char defout[14] = "roomassign.dat";
	
	clock_t begin , end;
	double time_spent;
	
	while((opt = getopt(argc, argv, "i:o:n:I:t:d:")) != -1)
	{
		switch(opt)
		{
			case 'i':
				ifile = strdup(optarg);
				break;

			case 'o':
				outfile = strdup(optarg);
				break;

			case 'n':
				n = atoi(optarg);
				break;

			case 'I':
				maxI_no_change = atoi(optarg);
				break;

			case 't':
				start_temp = atof(optarg);
				break;

			case 'd':
				dfile = strdup(optarg);
				break;

			case '?':
				if(optopt == 'i') ifile = strdup(defarg);
				if(optopt == 'o') outfile = strdup(defobj);
				if(optopt == 'n') n = 100;
				if(optopt == 'I') maxI_no_change = 1000;
				if(optopt == 't') start_temp = 1.0;
				if(optopt == 'd') dfile = strdup(defout);

				break;

			default: printf("Wrong Keyword\n");
				break;

		}

	}

//	printf("\nn = %d maxI_no_change = %d start temp = %f.\n",n,maxI_no_change,start_temp);getchar();

	begin = clock();

	ip_matrix = fopen( ifile, "rb");
	fread(&N, sizeof(int), 1, ip_matrix);//printf("\nN = %d.\n",N);getchar();

	if( n > N ) { printf("\n n = %d N = %d Data not sufficient.\n",n,N); return(0);}
	N = n;
	int dislike[N][N],tot_room_dislike;

	for (i = 0; i < N; i++) 
	{
		for (j = 0; j < N; j++) 
		{
			fread(&dislike[i][j], sizeof(int), 1, ip_matrix);	
//			printf("%d \n", dislike[i][j]);
		}
//		printf("\n");
	}	
//	getchar();
	fclose(ip_matrix);
	int cur_assignment[N], try_assignment[N];

	for(i = 0; i < N; i++) cur_assignment[i]=i/2;
	printf("Initial Room assignments:");
	for(i = 0; i < N; i++) printf("%d ", cur_assignment[i]);
	printf("\n");
//	getchar();

	for (i = 0; i < N-1; i++) 
	{
		swap = random_gen() * (N-i);

		temp = cur_assignment[swap];
		cur_assignment[swap] = cur_assignment[N-i-1];
		cur_assignment[N-i-1] = temp;
	}

	total_dislike = 0;
	for (i = 0; i < N; i++) 
	{
		for (j = 0; j < N; j++) 
		{
			if (cur_assignment[i] == cur_assignment[j])
			{ 
				total_dislike += dislike[i][j];
			}
   		}
	}

	T = start_temp;
	no_change_flg = 0;
	FILE *fp;
	fp = fopen( outfile, "wb+");
int z;
	fwrite(&n, sizeof(i), 1, fp);

//	for (i = 0; i < 1000; i++)
	for (i = 0; i < max_iterations; i++) 
	{

	fwrite(&i,	       sizeof(i), 	      1, fp);
	fwrite(&T,	       sizeof(T), 	      1, fp);
	fwrite(&total_dislike, sizeof(total_dislike), 1, fp);
	for (z=0; z < N; z++) fwrite(&cur_assignment[z], sizeof(cur_assignment[z]), 1, fp);

		if (++no_change_flg >= maxI_no_change) break;
	
		for (j=0; j < N; j++) try_assignment[j] = cur_assignment[j];
	
		do{
			roommate1 = random_gen() * N;
			roommate2 = random_gen() * N;
		}while (cur_assignment[roommate1] == cur_assignment[roommate2]); 

		temp = try_assignment[roommate1];
		try_assignment[roommate1] = try_assignment[roommate2];
		try_assignment[roommate2] = temp;

		new_dislike = 0.0;
		for (k = 0; k < N; k++)
		{
			for (j = 0; j < N; j++)
			{
				if (try_assignment[k] == try_assignment[j]) 
				{
					new_dislike += dislike[k][j];
				}
			}
		}
		double exponent = exp((double)((total_dislike - new_dislike) / T ));

		if ( (new_dislike < total_dislike) || (random_gen() <= exponent) ) 
		{
			for (j = 0; j < N; j++) cur_assignment[j]=try_assignment[j];
		
			printf("new %d, old %d ", new_dislike, total_dislike);
			total_dislike = new_dislike;
			printf("Arrangement %d: ",i);
	
	//		for (j = 0; j < N; j++) printf("%d ", cur_assignment[j]);
	//		printf("\n");	

			printf("no_change_flg=%d\n", no_change_flg);
			no_change_flg = 0;
		}
		
		T = 0.999 * T;
	}
	fclose(fp);	
	iterations_used = i;
	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	
	fp = fopen( dfile, "wb+");
	for (i = 0; i < N; i++) 
	{
		for (j = i+1; j < N; j++)
		{
			if (cur_assignment[i] == cur_assignment[j]) 
			{
				fwrite(&i, sizeof(i), 1, fp);
				fwrite(&j, sizeof(j), 1, fp);
				fwrite(&cur_assignment[i], sizeof(cur_assignment[i]), 1, fp);

tot_room_dislike = dislike[i][j] * 2;

				fwrite(&tot_room_dislike, sizeof(tot_room_dislike), 1, fp);

printf("%2d and %2d assigned to room %d total dislike factor = 2 * %d = %d\n",i,j,cur_assignment[i],dislike[i][j],tot_room_dislike);

			}
		}	
	}
	fclose(fp);
//	printf("Final Room cur_assignments:");
//	for (i=0; i<N; i++) printf("%d ", cur_assignment[i]);
//	printf("\n");

	printf("Total Dislike for this arrangement = %d\n", total_dislike);
	printf("Total iterations used to convergence: %d\n", iterations_used);
	printf("Total Elapsed time=%f\n\n",time_spent);
/*	fp = fopen( dfile, "wb+");

	for (i=0; i<N; i++) fwrite(&cur_assignment[i], sizeof(cur_assignment[i]), 1, fp);
	fclose(fp);
*/
	return 0;

}

