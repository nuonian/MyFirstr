#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define SIZE 1024
#define FROM_MASTER 1 
#define FROM_WORKER 2
#define DEBUG 0
MPI_Status status;
static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];
static void
init matrix(void)
{
    int i, j;
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
            a[i][j] = 1.0;
            b[i][j] = 1.0;
        }
}
static void
print matrix(void)
{
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
            printf(" %f", c[i][j]);
        printf("\n");
    }
}
int
main(int argc, char ∗∗argv)
{
    int myrank, nproc;
    int rows;
    int mtype;
    int dest, src, offset;
    double start_time, end_time;
    int i, j, k;
    MPI_Init(&argc, &argv);
    MPI_Comm size(MPI_COMM_WORLD, &nproc);
    MPI_Comm rank(MPI_COMM_WORLD, &myrank);
    if (myrank == 0) {
        printf("SIZE = %d, number of nodes = %d\n", SIZE, nproc);
        init matrix();
        start_time = MPI_Wtime();
        rows = SIZE / nproc;
        mtype = FROM_MASTER;
        offset = rows;
        for (dest = 1; dest < nproc; dest++) {
            if (DEBUG)
                printf(" sending %d rows to task %d\n", rows, dest);
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&a[offset][0], rows∗SIZE, MPI_DOUBLE, dest, mtype,
                MPI_COMM_WORLD);
            MPI_Send(&b, SIZE∗SIZE, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
            offset += rows;
        }
        for (i = 0; i < rows; i++) {
            for (j = 0; j < SIZE; j++) {
                c[i][j] = 0.0;
                for (k = 0; k < SIZE; k++)
                    c[i][j] = c[i][j] + a[i][k] ∗ b[k][j];
            }
        }
        mtype = FROM_WORKER;
        for (src = 1; src < nproc; src++) {
            MPI_Recv(&offset, 1, MPI_INT, src, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, src, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&c[offset][0], rows∗SIZE, MPI_DOUBLE, src, mtype,
                MPI_COMM_WORLD, &status);
            if (DEBUG)
                printf(" recvd %d rows from task %d, offset = %d\n",
                    rows, src, offset);
        }
        end_time = MPI_Wtime();
        if (DEBUG)
            print matrix();
        printf("Execution time on % 2d nodes: %f\n", nproc, end_time, start_time);
    }
    else {
        mtype = FROM MASTER;
        MPI_Recv(&offset, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&a[offset][0], rows∗SIZE, MPI_DOUBLE, 0, mtype,
            MPI_COMM_WORLD, &status);
        MPI_Recv(&b, SIZE∗SIZE, MPI_DOUBLE, 0, mtype, MPI_COMM WORLD,
            &status);
        if (DEBUG)
            printf("Rank = %d, offset = %d, row = %d, a[offset][0] = %e, b[0][0] = %e\n",
                myrank, offset, rows, a[offset][0], b[0][0]);
        for (i = offset; i < offset + rows; i++)
            for (j = 0; j < SIZE; j++) {
                c[i][j] = 0.0;
                for (k = 0; k < SIZE; k++)
                    c[i][j] = c[i][j] + a[i][k] ∗ b[k][j];
            }
        if (DEBUG)
            printf("Rank = %d, offset = %d, row = %d, c[offset][0] = %e\n",
                myrank, offset, rows, a[offset][0]);
        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD);
        MPI_Send(&c[offset][0], rows∗SIZE, MPI_DOUBLE, 0, mtype,
            MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
