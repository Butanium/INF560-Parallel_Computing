#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <stdint.h>

void pcg32_srandom(uint64_t seed)
{
    srand48(seed);
}

uint32_t pcg32_random_r()
{
    return (uint32_t)lrand48();
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv); // Initialize MPI
    double start = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get current process id
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes

    // Check the number of arguments
    if (argc != 3)
    {
        printf("Usage: %s <seed> <number of elements>\n", argv[0]);
        return 1;
    }
    // printf("Rank %d of %d\n", rank, size);

    // Get the arguments
    int seed = atoi(argv[1]);
    int n = atoi(argv[2]);
    if (rank == 0)
    {
        // Initialize the pseudo-random number generator
        pcg32_srandom(seed);
        for (int rcv = 1; rcv < size; rcv++)
        {
            int chunk = n / size;
            // Skip the elements that are not in the current chunk
            if (rcv == size - 1)
            {
                chunk = n - rcv * chunk;
            }
            // Allocate the array
            int *array = (int *)malloc(chunk * sizeof(int));

            // Initialize the array with random values
            for (int i = 0; i < chunk; i++)
            {
                array[i] = pcg32_random_r();
            }
            MPI_Send(array, chunk, MPI_INT, rcv, 0, MPI_COMM_WORLD);
            fflush(stdout);
            free(array);
        }
    }
    else
    {
        int chunk = n / size;
        // Skip the elements that are not in the current chunk
        if (rank == size - 1)
        {
            chunk = n - rank * chunk;
        }
        // Allocate the array
        int *array = (int *)malloc(chunk * sizeof(int));
        // printf("Receiving %d elements as rank %d\n", chunk, rank);
        MPI_Recv(array, chunk, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("Received %d elements as rank %d\n", chunk, rank);
        fflush(stdout);
        int max = 0;
        for (int i = 0; i < chunk; i++)
        {
            if (array[i] > max)
            {
                max = array[i];
            }
        }
        MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(array);
    }

    if (rank == 0)
    {
        int received = 0;
        int max;
        while (received < size - 1)
        {
            int buffer;
            MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (buffer > max || received == 0)
            {
                max = buffer;
            }
            received++;
        }
        printf("Max value: %d\n", max);
        double end = MPI_Wtime();
        double time = end - start;
        printf("Time: %f\n", time);
    }
    // Terminate MPI
    MPI_Finalize();
    return 0;
}