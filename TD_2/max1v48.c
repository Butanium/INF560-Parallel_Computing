#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdint.h>
#include <time.h>

void pcg32_srandom(uint64_t seed)
{
    srand48(seed);
}

uint32_t pcg32_random_r()
{
    return (uint32_t)lrand48();
}

void pcg32_advance(int64_t delta)
{
    // No need to implement this function when using lrand48
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

    // Get the arguments
    int seed = atoi(argv[1]);
    int n = atoi(argv[2]);

    // Initialize the pseudo-random number generator
    pcg32_srandom(seed);

    // Allocate the array
    int *array = (int *)malloc(n * sizeof(int));

    // Initialize the array with random values
    for (int i = 0; i < n; i++)
    {
        array[i] = pcg32_random_r();
    }

    // Search for the maximum value
    int max = 0;
    for (int i = 0; i < n; i++)
    {
        if (array[i] > max)
        {
            max = array[i];
        }
    }
    double end = MPI_Wtime();
    double time = end - start;

    // Print the results
    printf("Max value: %d\n", max);
    printf("Time: %f\n", time);

    // Terminate MPI
    MPI_Finalize();
    return 0;
}