#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define THREADS 4
#define MESSAGES 5
#define ARR_SIZE 50

typedef struct {
    int thread_num;
    const char *messages[MESSAGES];
} thread_data_t;

typedef struct {
    int value;
} sleepsort_arg_t;

void cleanup_handler(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    printf("Поток %d: Выполняется очистка перед завершением\n", data->thread_num);
}

void* thread_func(void* arg) {
    thread_data_t *data = (thread_data_t *)arg;
    
    pthread_cleanup_push(cleanup_handler, data);
    
    for (int i = 0; i < MESSAGES; i++) {
        printf("Поток %d: %s\n", data->thread_num, data->messages[i]);
        sleep(1); 
    }
    
    pthread_cleanup_pop(0);
    printf ("Поток %d: завершение работы\n", data->thread_num);
    return NULL;
}

void* sleepsort_thread(void* arg) {
    sleepsort_arg_t* sleept = (sleepsort_arg_t*)arg;
    sleep(sleept->value);
    printf("%d ", sleept->value);
    free(sleept);
    return NULL;
}

void run_sleepsort(int arr[], int n) {
    if (n <= 0) {
        printf("Некорректный размер массива!\n");
        return;
    }

    pthread_t sleep_threads[ARR_SIZE];
    
    printf("\n--- Запуск прикольного Sleepsort ---\n");
    printf("     Исходный массив:     ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("Отсортированный массив:   ");
    
    for (int i = 0; i < n; i++) {
        sleepsort_arg_t* arg = malloc(sizeof(sleepsort_arg_t));
        if (!arg) {
            perror("Ошибка выделения памяти");
            exit(EXIT_FAILURE);
        }
        arg->value = arr[i];
        pthread_create(&sleep_threads[i], NULL, sleepsort_thread, arg);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(sleep_threads[i], NULL);
    }
    printf("\n--- Sleepsort завершен ---\n\n");
}

int main() {
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
    
    printf("--- Запуск потоков с Сообщениями ---\n");
    for (int i = 0; i < THREADS; i++) {
        thread_data[i].thread_num = i + 1;
        memcpy(thread_data[i].messages, messages[i], sizeof(messages[i]));
        pthread_create(&threads[i], NULL, thread_func, &thread_data[i]);
    }

    printf("\n\nЖдем 2 секунды...\n\n");
    sleep(2);

    printf("Отменяем все потоки с сообщениями...\n");
    for (int i = 0; i < THREADS; i++) {
        pthread_cancel(threads[i]);
    }

    printf("Ожидаем завершения потоков с сообщениями...\n");
    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--- Все потоки с сообщениями завершили работу ---\n");

    int sleeparr [ARR_SIZE];
    for (int i = 0; i < ARR_SIZE; i++) {
        sleeparr[i] = rand() % 15;
    }
    run_sleepsort(sleeparr, ARR_SIZE);

    printf("\nВсе задачи завершены.\n");
    return 0;
}