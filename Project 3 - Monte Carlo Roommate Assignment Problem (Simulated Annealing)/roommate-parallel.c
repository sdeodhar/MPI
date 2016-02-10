
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <getopt.h>
#include <string.h>

//#define RAND_MAX ((1U << 31) - 1)
/* 
inline int try_rand()
{
	return rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
}
 */
//#define RAND_MAX_32 ((1U << 31) - 1)
//#define RAND_MAX ((1U << 15) - 1)
 /*
inline int try_rand1()
{
	return (rseed = (rseed * 214013 + 2531011) & RAND_MAX_32) >> 16;
}*/

int random_gen(int rseed, int a, int c, int m, int rank)
{
	int rand_value;
	rand_value=(a*(rank+1)*rseed+c*(2*rank+2))%m;
//	value = value % 1000;
	return rand_value;
}

int main (int argc, char *argv[])
{
	int i,j,n,N,roommate1, roommate2; 
	double current_sum, new_sum; 	
	double tot_sum;	
//	double sum_comp[16];
	double t,start_temp,u , exponent;
//	double k;			// the constant factor that t will be reduced by after every iteration.
	int no_change_flg = 0, rank, size, temp,tot_room_dislike;
	FILE *matrix_input, *fp;
	int max_iter = 100000, iterations;		
	double start, finish;

	int opt,maxI_no_change;
        char *dfile, *outfile, *ifile;

	const char defarg[22] = "dislikes.dat";
	const char defobj[22] = "objfuncvalue.dat";
	const char defout[22] = "roomassign.dat";
	
//	clock_t pbegin, pend ,fl_begin , fl_end;
//	double time_spent,floyd_time_spent;
//      pbegin = clock();

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
				N = atoi(optarg);
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
				if(optopt == 'n') N = 100;
				if(optopt == 'I') maxI_no_change = 1000;
				if(optopt == 't') start_temp = 1.0;//strdup(defarg);
				if(optopt == 'd') dfile = strdup(defout);

				break;

			default: printf("Wrong Keyword\n");
				break;
		}
	}

//	printf("\nn = %d maxI_no_change = %d start temp = %f.\n",N,maxI_no_change,start_temp);getchar();

	MPI_Init(&argc,  &argv);
	MPI_Barrier(MPI_COMM_WORLD);

	start=MPI_Wtime();
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	matrix_input = fopen( ifile, "rb");
	fread(&n, sizeof(int), 1, matrix_input);

	if( N > n ) { printf("\n n = %d N = %d Data not sufficient.\n",n,N); return(0);}
	n = N;

	int dislike[n][n];	
	for (i=0; i<n; i++) 
	{
		for (j=0; j<n; j++)
		{
			fread(&dislike[i][j],sizeof(int), 1,matrix_input);
			//if (!rank) printf("%d ", dislike[i][j]);
		}
		//if (!rank) printf("\n");
	}
	int assigned[n], cur_assignment[n], try_assignment[n];
	int a,c,m,seed;	

//let
//a = 1103515245;	c = 12345;	m = RAND_MAX;	seed = 0;
//a = 214013;	c = 2531011;	m = RAND_MAX_32; seed = 0;

	a = 3493;	c = 1292;	m = 2939;	seed = 1421;

	for (i = 0 ; i < n; i++) assigned[i] = 0;
	iterations=0;

	for (i = 0; i < n/2; i++) 
	{
		do {
			iterations++;
			seed = random_gen(seed,a,c,m,rank);
			u = ((double)seed)/((double)m);
			roommate1 = ((int) (u * n));  

			seed = random_gen(seed,a,c,m,rank);
			u = ((double)seed) / ((double)m);
			roommate2 = ((int) (u * n));  

			if (iterations % 1000 == 0)
			{ 
				a = random_gen(seed,a,c,m,rank) + 1;
				c = random_gen(seed,a,c,m,rank) + 1;
			}

		} while( (roommate1 == roommate2) || (assigned[roommate1] == 1) || (assigned[roommate2] == 1) );

		cur_assignment[roommate1] = i;
		cur_assignment[roommate2] = i;
		assigned[roommate1] = 1;
		assigned[roommate2] = 1;
	}

/*
MPI_Barrier(MPI_COMM_WORLD);
for (i=0; i<size; i++) {
	if (rank==i) {
	printf("rank=%d cur_assignment= ", rank);
		for (j=0; j<n; j++) {
		printf("%d ", cur_assignment[j]);
		}
	printf("\n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
}
MPI_Barrier(MPI_COMM_WORLD);
*/
//printf("try_assignment= ");

	for (i = 0; i < n; i++) 
	{
		try_assignment[i] = cur_assignment[i];
	//	printf("%d ", try_assignment[i]);
	}
	//printf("\n");

	current_sum = 0.0;
	for (i = 0; i < n; i++) 
	{
		for (j = 0; j < n; j++) 
		{
			if (cur_assignment[i] == cur_assignment[j])
			{
				current_sum = current_sum + dislike[i][j];
			}
		}
	}

//printf("rank=%d initial_dislike=%f\n", rank, current_sum);

//	k = 0.999;
	t = start_temp;

if ( rank == 0 ) fp = fopen( "objrank0.dat", "wb+");
if ( rank == 1 ) fp = fopen( "objrank1.dat", "wb+");
if ( rank == 2 ) fp = fopen( "objrank2.dat", "wb+");
if ( rank == 3 ) fp = fopen( "objrank3.dat", "wb+");
if ( rank == 4 ) fp = fopen( "objrank4.dat", "wb+");
if ( rank == 5 ) fp = fopen( "objrank5.dat", "wb+");
if ( rank == 6 ) fp = fopen( "objrank6.dat", "wb+");
if ( rank == 7 ) fp = fopen( "objrank7.dat", "wb+");
if ( rank == 8 ) fp = fopen( "objrank8.dat", "wb+");
if ( rank == 9 ) fp = fopen( "objrank9.dat", "wb+");
if ( rank == 10) fp = fopen( "objrank10.dat","wb+");
if ( rank == 11) fp = fopen( "objrank11.dat","wb+");
if ( rank == 12) fp = fopen( "objrank12.dat","wb+");
if ( rank == 13) fp = fopen( "objrank13.dat","wb+");
if ( rank == 14) fp = fopen( "objrank14.dat","wb+");
if ( rank == 15) fp = fopen( "objrank15.dat","wb+");

int z;
fwrite(&n, sizeof(n), 1, fp);

	while ((no_change_flg < maxI_no_change) && (iterations < max_iter))
	{     

	fwrite(&iterations,    sizeof(iterations),    1, fp);
	fwrite(&t,	       sizeof(t), 	      1, fp);
	fwrite(&current_sum,   sizeof(current_sum),   1, fp);
	for (z=0; z < N; z++) fwrite(&cur_assignment[z], sizeof(cur_assignment[z]), 1, fp);

		iterations++;
//		printf("\niteration = %d\n",iterations);

		do {
			seed=random_gen(seed,a,c,m,rank);
			u=((double)seed)/((double)m);
			roommate1= ((int) (u*n));

			seed=random_gen(seed,a,c,m,rank);
			u=((double)seed)/((double)m);
			roommate2= ((int) (u*n));

		} while (cur_assignment[roommate1]==cur_assignment[roommate2]);
//	printf("rank=%d c1=%d c2=%d\n", rank, roommate1, roommate2);

		temp=try_assignment[roommate1];
		try_assignment[roommate1]=try_assignment[roommate2];
		try_assignment[roommate2]=temp;

		new_sum=0.0;
		for (i=0; i<n; i++) 
		{
			for (j=0; j<n; j++) 
			{
				if (try_assignment[i]==try_assignment[j])
				{ 
					new_sum=new_sum+dislike[i][j];
				}
			}
		}

//printf("rank=%d new_sum=%f\n",rank,new_sum);

		seed=random_gen(seed,a,c,m,rank); 

		u=((double)seed) / ((double) m);
		exponent=exp((double)((current_sum-new_sum)/t));

//printf("u=%f exponent=%f t=%f", u, exponent, t);
		if ((new_sum < current_sum) || (((u <= exponent)) && (current_sum - new_sum > 0.0000001)) ) 
		{
			temp = cur_assignment[roommate1];
			cur_assignment[roommate1] = cur_assignment[roommate2];
			cur_assignment[roommate2] = temp;
			no_change_flg = 0;
			current_sum = new_sum;
//printf("rank= %d total dislikeness=%f\n", rank, current_sum);
		}

		else  
		{
			no_change_flg++;
		
			temp=try_assignment[roommate1];
			try_assignment[roommate1]=try_assignment[roommate2];
			try_assignment[roommate2]=temp;
		} 
	
		t = 0.999 * t;
		if (iterations%10000==0)
		{ 
			a=random_gen(seed,a,c,m,rank)+1;
			c=random_gen(seed,a,c,m,rank)+1;
		}
	} 
	fclose(fp);
	MPI_Barrier(MPI_COMM_WORLD);
	printf("rank=%d has global sum = %f after %d runs\n",rank,current_sum,iterations);

	MPI_Reduce(&current_sum, &tot_sum, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Bcast(&tot_sum,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
//sum_comp[ rank ] = tot_sum;

	finish=MPI_Wtime();

	double elapsed_time=(finish-start);
	//printf("\n");


	/*
	MPI_Barrier(MPI_COMM_WORLD);
	for (i=0; i<size; i++) {
		if (rank==i) {
		printf("rank=%d minimum_dislikeness=%f cur_assignment= ", rank,current_sum);
			for (j=0; j<n; j++) {
			printf("%d ", cur_assignment[j]);
			}
	//	printf("\n");
		}
	MPI_Barrier(MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	*/
//	if (!rank) printf("\nTotal minimum dislikeness: %f\n",tot_sum);

/*	if (current_sum==tot_sum) {
		printf("\nid=%d sum=%f cur_assignment= ",rank, tot_sum);
		for (j=0; j<n; j++) {printf("%d ", cur_assignment[j]); if(!(j%10)) printf("\n");}
	printf("\n");
	}*/

	MPI_Barrier(MPI_COMM_WORLD);
	
	if (!rank) 
	{fp = fopen( dfile, "wb+");
		for (i=0; i<n; i++)
		{
			for (j=i+1; j<n; j++)
			{
				if (cur_assignment[i]==cur_assignment[j])
				{
				fwrite(&i, sizeof(i), 1, fp);
				fwrite(&j, sizeof(j), 1, fp);
				fwrite(&cur_assignment[i], sizeof(cur_assignment[i]), 1, fp);

tot_room_dislike = dislike[i][j] * 2;

				fwrite(&tot_room_dislike, sizeof(tot_room_dislike), 1, fp);
				printf("persons %d and %d were assigned to room %d dislike = %d\n",i,j, cur_assignment[i],tot_room_dislike);
				}
			}
		}
		fclose(fp);

		printf("Number of processors used: %d\n", size);
//		printf("Total iterations used: %d\n", iterations);
		printf("Total Elapsed time=%f\n\n",elapsed_time);
		printf("\nTotal minimum dislikeness: %f\n",tot_sum);
//		printf("\nFor task %d Total minimum dislikeness: %f\n",rank,sum_comp[rank]);
/*		fp = fopen( dfile, "wb+");

		for (i=0; i<N; i++) fwrite(&cur_assignment[i], sizeof(cur_assignment[i]), 1, fp);
		fclose(fp);
*/
	}
//	printf("\nFor task %d Total minimum dislikeness: %f\n",rank,sum_comp[rank]);
	MPI_Finalize();

	return 0;
}

