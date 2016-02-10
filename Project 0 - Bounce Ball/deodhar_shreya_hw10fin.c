#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char *argv[])
{
        int rank, size;
        int ball_value = 0;
        int ball_present = 1;
        int delta,another,next_task,i;

	MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        
	srandom(rank);

        delta = random();
        delta = delta % 13;
	
        another = random();
        another = another % 5;
	
        if (another == 0)
	{
        	delta = delta * (-1);
	}
	if (rank == 0)			
        {
                //printf("task 0 Initializing ball\n");
                ball_present = 1;
        }

	while(1)			
        {
		if (ball_present == 0)
		{
                	MPI_Recv(&ball_value, sizeof(ball_value), MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			printf("Task %d has the Ball with value %d \n",rank, ball_value);

			ball_present = 1;

			if (ball_value >= 1000)
			{
				break;
			}
                }

		ball_value = ball_value + delta;

                if (ball_value > 100)  
                {
                        for(i=0; i<size; i++)
                        {
                                if (i != rank)
                                {
                                        ball_value = 1000;
					//printf("i = %d\n",i);
                                        MPI_Send(&ball_value, sizeof(ball_value), MPI_INT, i, 1, MPI_COMM_WORLD);
                                }
                        }
                        break;
                }  

                next_task = random();	
                next_task = (next_task) % size;

                if (next_task == rank)
                {
		}
		else
		{			
			MPI_Send(&ball_value, sizeof(ball_value), MPI_INT, next_task, 1, MPI_COMM_WORLD);
                	ball_present = 0;
		}
	}

        printf("Ball Dropped\n");
	MPI_Finalize();
        return 0;
}
