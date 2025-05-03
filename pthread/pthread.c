#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define THREADS 4
#define MESSAGES 5
#define ARR_SIZE 50
#define MAX_MATRIX_SIZE 10

typedef struct {
    int thread_num;
    const char *messages[MESSAGES];
    pthread_mutex_t *mutex;
} thread_data_t;

typedef struct {
    int value;
    pthread_mutex_t *mutex;
} sleepsort_arg_t;

typedef struct {
    int thread_id;
    int num_threads;
    int N;
    int **A;
    int **B;
    int **C;
    int rows_per_thread;
    int extra_rows;
} matrix_thread_data_t;

pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;
int current_thread = 0;

void cleanup_handler(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    printf("Поток %d: Выполняется очистка перед завершением\n", data->thread_num);
}

void* thread_func(void* arg) {
    thread_data_t *data = (thread_data_t *)arg;
    
    pthread_cleanup_push(cleanup_handler, data);
    
    for (int i = 0; i < MESSAGES; i++) {
        pthread_mutex_lock(data->mutex);
        while (current_thread != data->thread_num - 1) {
            pthread_mutex_unlock(data->mutex);
            usleep(100);
            pthread_mutex_lock(data->mutex);
        }
        printf("Поток %d: %s\n", data->thread_num, data->messages[i]);
        current_thread = (current_thread + 1) % THREADS;
        pthread_mutex_unlock(data->mutex);
        sleep(1); 
    }
    
    pthread_cleanup_pop(0);
    printf("Поток %d: завершение работы\n", data->thread_num);
    return NULL;
}


void* sleepsort_thread(void* arg) {
    sleepsort_arg_t* sleept = (sleepsort_arg_t*)arg;
    sleep(sleept->value);
    pthread_mutex_lock(sleept->mutex);
    printf("%d ", sleept->value);
    pthread_mutex_unlock(sleept->mutex);
    free(sleept);
    return NULL;
}

void run_sleepsort(int arr[], int n) {
    if (n <= 0) {
        printf("Некорректный размер массива!\n");
        return;
    }

    pthread_t sleep_threads[ARR_SIZE];
    
    printf("\n\n\n                                    --- Запуск Sleepsort ---\n\n");
    printf("Исходный массив:        ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("Отсортированный массив: ");
    
    for (int i = 0; i < n; i++) {
        sleepsort_arg_t* arg = malloc(sizeof(sleepsort_arg_t));
        if (!arg) {
            perror("Ошибка выделения памяти");
            exit(EXIT_FAILURE);
        }
        arg->value = arr[i];
        arg->mutex = &main_mutex;
        pthread_create(&sleep_threads[i], NULL, sleepsort_thread, arg);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(sleep_threads[i], NULL);
    }
    sleep(1);
    printf("\n\n--- Sleepsort завершен ---\n\n\n\n");
}


void init_matrix(int **matrix, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = 1;
        }
    }
}

void print_matrix(int **matrix, int N, const char* color, const char* name) {
    printf("\n%s \n", color);
    printf("\n ________________________________________\n");
        printf("| %-34s        |\n", name);
        printf("|________________________________________|\n");
    
    for (int i = 0; i < N; i++) {
        printf("| ");
        for (int j = 0; j < N; j++) {
            printf("%3d ", matrix[i][j]);
        }
        
        for (int j = N; j < 8; j++) printf("    ");
        printf("   |\n");
    }
    printf("|________________________________________|%s\n", COLOR_RESET);
}

void* matrix_multiply_part(void* arg) {
    matrix_thread_data_t* data = (matrix_thread_data_t*)arg;
    int rows_per_thread = data->rows_per_thread;
    int extra_rows = data->extra_rows;
    
    int start = data->thread_id * rows_per_thread;
    int end = start + rows_per_thread;
    
    if (data->thread_id == data->num_threads - 1) {
        end += extra_rows;
    }

    for (int i = start; i < end; i++) {
        for (int j = 0; j < data->N; j++) {
            data->C[i][j] = 0;
            for (int k = 0; k < data->N; k++) {
                data->C[i][j] += data->A[i][k] * data->B[k][j];
            }
        }
    }
    return NULL;
}

void run_matrix_multiplication(int N, int num_threads) {

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

    int rows_per_thread = N / num_threads;
    int extra_rows = N % num_threads;

    pthread_t threads[num_threads];
    matrix_thread_data_t thread_data[num_threads];

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].N = N;
        thread_data[i].A = A;
        thread_data[i].B = B;
        thread_data[i].C = C;
        thread_data[i].rows_per_thread = rows_per_thread;
        thread_data[i].extra_rows = extra_rows;
        pthread_create(&threads[i], NULL, matrix_multiply_part, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (N < 10) {
        print_matrix(A, N, COLOR_BLUE,    "Матрица A                      "); sleep(1);
        print_matrix(B, N, COLOR_GREEN,   "Матрица B                      "); sleep(1);
        print_matrix(C, N, COLOR_MAGENTA, "Результат умножения (матрица C)"); sleep(1);
        
    } 

    for (int i = 0; i < N; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
}

int main(int argc, char* argv[]) {
    
    srand(time(NULL));
    pthread_t threads[THREADS];
    thread_data_t thread_data[THREADS];
    
    const char *messages[THREADS][MESSAGES] = {
        {"Сообщение 1 потока 1", "Сообщение 2 потока 1", 
         "Сообщение 3 потока 1", "Сообщение 4 потока 1", 
         "Сообщение 5 потока 1"},

        {"Сообщение 1 потока 2", "Сообщение 2 потока 2",
         "Сообщение 3 потока 2", "Сообщение 4 потока 2",
         "Сообщение 5 потока 2"},

        {"Сообщение 1 потока 3", "Сообщение 2 потока 3",
         "Сообщение 3 потока 3", "Сообщение 4 потока 3",
         "Сообщение 5 потока 3"},

        {"Сообщение 1 потока 4", "Сообщение 2 потока 4",
         "Сообщение 3 потока 4", "Сообщение 4 потока 4",
         "Сообщение 5 потока 4"}
    };
    
    printf("\x1b[36m--- Запуск потоков с Сообщениями ---\x1b[0m\n  ");
    for (int i = 0; i < THREADS; i++) {
        thread_data[i].thread_num = i + 1;
        memcpy(thread_data[i].messages, messages[i], sizeof(messages[i]));
        thread_data[i].mutex = &main_mutex;
        pthread_create(&threads[i], NULL, thread_func, &thread_data[i]);
    }

    printf("\n\n  Ожидание 2 секунды...\n\n");
    sleep(2);

    printf("Отменяем все потоки с сообщениями...\n");
    for (int i = 0; i < THREADS; i++) {
        pthread_cancel(threads[i]);
    }

    printf("\x1b[36mОжидаем завершения потоков с сообщениями...\x1b[0m\n  \n");
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- Все потоки с сообщениями завершили работу ---");

    int sleeparr[ARR_SIZE];
    for (int i = 0; i < ARR_SIZE; i++) {
        sleeparr[i] = rand() % 10;
    }
    run_sleepsort(sleeparr, ARR_SIZE);


    if (argc == 3) {
        int N = atoi(argv[1]);
        int num_threads = atoi(argv[2]);
        
        if (N <= 0 || num_threads <= 0) {
            printf("%sОшибка:%s размер матрицы и количество потоков должны быть положительными\n", 
                   COLOR_RED, COLOR_RESET);
        } else if (N > MAX_MATRIX_SIZE) {
            printf("Ошибка: максимальный размер матрицы %d\n");
        } else {
            printf("\n=== Запуск умножения матриц %dx%d с %d потоками ===\n", N, N, num_threads);
            run_matrix_multiplication(N, num_threads);
        }
    } else {
        printf("\nИспользование: %s <размер матрицы> <количество потоков>\n", argv[0]);
    }


    printf("\n\n%s        Все задачи завершены %s \n", COLOR_GREEN, COLOR_RESET);
    sleep(2);
    system("python graph.py");
    return 0;

}
