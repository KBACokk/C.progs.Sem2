#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAX_THREADS 128
#define MAX_MATRIX_SIZE 2000

typedef struct {
    int thread_id;
    int num_threads;
    int N;
    int **A;
    int **B;
    int **C;
    int start_row;
    int end_row;
} matrix_thread_data_t;

long long current_time_in_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void init_matrix(int **matrix, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void* matrix_multiply_part(void* arg) {
    matrix_thread_data_t* data = (matrix_thread_data_t*)arg;
    
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < data->N; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < data->N; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
    return NULL;
}

long long measure_matrix_multiplication(int N, int num_threads) {

    int **A = (int**)malloc(N * sizeof(int*));
    int **B = (int**)malloc(N * sizeof(int*));
    int **C = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        A[i] = (int*)malloc(N * sizeof(int));
        B[i] = (int*)malloc(N * sizeof(int));
        C[i] = (int*)malloc(N * sizeof(int));
    }

    init_matrix(A, N);
    init_matrix(B, N);


    pthread_t threads[MAX_THREADS];
    matrix_thread_data_t thread_data[MAX_THREADS];

    int rows_per_thread = N / num_threads;
    int extra_rows = N % num_threads;
    int current_row = 0;

    long long start_time = current_time_in_ms();

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].N = N;
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        
        thread_data[i].start_row = current_row;
        
        thread_data[i].end_row = current_row + rows_per_thread;
        if (i < extra_rows) {
            thread_data[i].end_row + 1;
        }
        current_row = thread_data[i].end_row;
        
        pthread_create(&threads[i], NULL, matrix_multiply_part, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    long long end_time = current_time_in_ms();


    for (int i = 0; i < N; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    return end_time - start_time;
}

int main() {

    int matrix_sizes[] = {100, 250, 500, 750, 1000, 1250, 1500, 1750, 2000};
    int num_sizes = sizeof(matrix_sizes) / sizeof(matrix_sizes[0]);
    

    int thread_counts[] = {1, 2, 4, 8, 16, 32, 64, 128};
    int num_thread_counts = sizeof(thread_counts) / sizeof(thread_counts[0]);




    for (int i = 0; i < num_sizes; i++) {
        int N = matrix_sizes[i];
        printf("%d", N);
        
        printf("Testing matrix size: %dx%d\n", N, N);
        
        for (int j = 0; j < num_thread_counts; j++) {
            int threads = thread_counts[j];
            
            if (threads > N) {
                printf("  Threads: %d - skipped (more threads than rows)\n", threads);
                continue;
            }
            
            long long time_taken = measure_matrix_multiplication(N, threads);

            printf("  Threads: %d, Time: %lld ms\n", threads, time_taken);
        }
        printf("\n");
    }



    return 0;
}