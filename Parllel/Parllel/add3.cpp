#include <mpi.h>
#include <algorithm>
#include <fstream>
#include <cmath>

const int root_id = 0;
const int max_procs_size = 16;

int main(int argc, char* argv[])
{
    double start_time, end_time, time;
    int procs_id, procs_size;
    MPI_Status status;
    MPI_Request reqSend, reqRecv;
    MPI_Init(&argc, &argv);
    start_time = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &procs_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &procs_id);

    int N = 0;
    {
        for (int i = 1; i < argc; ++i) {
            char* pos = strstr(argv[i], "-N=");
            if (pos != NULL) {
                sscanf(pos, "-N=%d", &N);
                break;
            }
        }
    }

    const int procs_size_sqrt = floor(sqrt(static_cast<double>(procs_size)));
    const int n = N / procs_size_sqrt;
    const int n_sqr = n * n;

    if (procs_size<4 || procs_size> max_procs_size) {
        printf("The fox algorithm requires at least 4 processors and at most %d processors. ",
            max_procs_size);
        MPI_Finalize();
        return 0;
    }
    if (procs_size_sqrt * procs_size_sqrt != procs_size) {
        printf("The number of process must be a square. ");
        MPI_Finalize();
        return 0;
    }
    if (N % procs_size_sqrt != 0) {
        printf("N mod procs_size_sqrt !=0  ");
        MPI_Finalize();
        return 0;
    }

    int* A = (int*)malloc(sizeof(int) * n_sqr);
    int* B = (int*)malloc(sizeof(int) * n_sqr);
    int* C = (int*)malloc(sizeof(int) * n_sqr);
    int* T = (int*)malloc(sizeof(int) * n_sqr);
    

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            A[i * n + j] = (i + j) * procs_id;
            B[i * n + j] = (i - j) * procs_id;
            C[i * n + j] = 0;
        }

    printf("A on procs %d :\n", procs_id);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%5d", A[i * n + j]);
        }
        printf("\n");
    }

    printf("B on procs %d :\n", procs_id);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%5d", B[i * n + j]);
        }
        printf("\n");
    }

    MPI_Comm cart_all, cart_row, cart_col;
    int dims[2], periods[2];
    int procs_cart_rank, procs_coords[2];
    dims[0] = dims[1] = procs_size_sqrt;
    periods[0] = periods[1] = true;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, false, &cart_all);
    MPI_Comm_rank(cart_all, &procs_cart_rank);
    MPI_Cart_coords(cart_all, procs_cart_rank, 2, procs_coords);
    MPI_Comm_split(cart_all, procs_coords[0], procs_coords[1], &cart_row);
    MPI_Comm_split(cart_all, procs_coords[1], procs_coords[0], &cart_col);
    int rank_cart_row, rank_cart_col;
    MPI_Comm_rank(cart_row, &rank_cart_row);
    MPI_Comm_rank(cart_col, &rank_cart_col);


    for (int round = 0; round < procs_size_sqrt; ++round) {

        MPI_Isend(B, n_sqr, MPI_INT, (procs_coords[0] - 1 + procs_size_sqrt) % procs_size_sqrt,
            1, cart_col, &reqSend);

        int broader = (round + procs_coords[0]) % procs_size_sqrt;
        if (broader == procs_coords[1]) std::copy(A, A + n_sqr, T);

        MPI_Bcast(T, n_sqr, MPI_INT, broader, cart_row);

        for (int row = 0; row < n; ++row)
            for (int col = 0; col < n; ++col)
                for (int k = 0; k < n; ++k) {
                    C[row * n + col] += T[row * n + k] * B[k * n + col];
                }

        MPI_Wait(&reqSend, &status);
        MPI_Recv(T, n_sqr, MPI_INT, (procs_coords[0] + 1) % procs_size_sqrt
            , 1, cart_col, &status);
        std::copy(T, T + n_sqr, B);

    }

    printf("C on procs %d :  \n", procs_id);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%5d", C[i * n + j]);
        }
        printf("\n");
    }


    MPI_Comm_free(&cart_col);
    MPI_Comm_free(&cart_row);
    MPI_Comm_free(&cart_all);
    free(A);
    free(B);
    free(C);
    free(T);
    end_time = MPI_Wtime();
    MPI_Finalize();
    printf("task %d consumed %lf seconds \n", procs_id, end_time - start_time);

    return 0;
}