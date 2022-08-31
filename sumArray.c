// importing the important libraries
#include <stdio.h>
#include <mpi.h>

#define NUMDATA 10000
int data[NUMDATA];
// function which will be called by all the processes
void LoadData(int data[])
{
    for (int i = 0; i < NUMDATA; i++)
    {
        data[i] = 1;
    }
}
// function to add the data
int AddUp(int data[], int count)
{
    int sum = 0;
    for (int i = 0; i < count; i++)
    {
        sum += data[i];
    }
    return sum;
}
// main function
int main(void)
{
    int sum;
    int size;
    int rank;
    int tag = 0;
    int chunksize;
    int start;
    int result;

    chunksize = NUMDATA / size;
    // initializing the MPI environment
    MPI_Init(NULL, NULL);
    // getting the size of the communicator
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // getting the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // loading the data
    chunksize = NUMDATA / size;
    if (rank == 0)
    {
        LoadData(data);
        for (int i = 1; i < size; i++)
        {
            start = i * chunksize;
            // sending the data to the other processes
            MPI_Send(&(data[start]), chunksize, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
        sum = AddUp(data, chunksize);
        for (int i = 1; i < size; i++)
        {
            // receiving the data from the other processes
            MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, tag,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += result;
        }
        printf("Total sum is %d\n", sum);
    }
    else
    {
        // receiving the data from the process with rank 0
        MPI_Recv(data, chunksize, MPI_INT, 0, tag, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        sum = AddUp(data, chunksize);
        // sending the data to the process with rank 0
        MPI_Send(&sum, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    // finalizing the MPI environment
    MPI_Finalize();
    return 0;
}