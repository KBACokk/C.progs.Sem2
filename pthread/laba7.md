# 🚀 Знакомство с POSIX потоками

**Краткое описание**: В данном проекте показано освоение темы многопоточности, реализованной на языке программирования C.

---

## 🔥 Основные пункты 
- ✅ Первый этап. Реализация работы на 60 баллов ( оценка 3 )
- ✅ Второй этап. Реализация работы на 80 баллов ( оценка 4 )

---

## 💻 Первый этап. Знакомство с pthread:
---

### 1.	Создать поток
*Пояснение - Для начала создадим два потока, которые будут являться дочерним и родительским, относительно приоритета работы.*


![image](https://github.com/user-attachments/assets/4822e4a1-58f6-4f9c-b323-a3ddbe10ab14)

---

### 2.	Ожидание потока
*Пояснение - Изменим приоритет и обозначим, что дочерний поток должен выполнить свою программу, раньше основной “родительской программы”, для этого изменим pthread_join(thread, NULL) на более раннюю позицию, перед основной программой, что даст ей приоритет в выполнении.*


![image](https://github.com/user-attachments/assets/3f56d781-3127-4126-9931-8cc53f941028)

---

### 3.	Параметры потока
*Пояснение - Изменим программу, чтобы она могла сама создавать необходимо количество потоков, каждый из которых будет в свою очередь выводить свой номер и сообщение своего потока.*


![image](https://github.com/user-attachments/assets/6a578fbe-cef2-490d-b490-7352819a4c39)

---

### 4.	Завершение нити без ожидания
*Пояснение - Изменим вывод функции,  добавив параметр sleep, определяющий, что программа должна прождать 1 секунду между каждым новым выводом потока, а так же внесём параметр sleep, для основной программы, который будет отвечать за то, что через 2 секунды, основной поток прервёт работу всех существующих потоков.*


![image](https://github.com/user-attachments/assets/50dd0c72-310d-445e-938e-d039c1c0f2e6)

---

### 5.	Обработать завершение потока
*Пояснение - С помощью pthread_cleanup_push(), мы инициализируем завершение работы потоков, выводя об этом сообщения в терминал.*


![image](https://github.com/user-attachments/assets/15df5edf-27e8-4c85-8421-df52b51965f2)

---

### 6.	Реализовать простой Sleepsort
*Пояснение - Создадим сортировку Sleep_Sort, основанную на принципе вывода значения своего аргумента спустя время, равное аргументу элемента массива.*


![image](https://github.com/user-attachments/assets/cc11821c-8f83-42d7-9aa5-6514596f601c)

---

## 💻 Второй этап. Перемножение матриц:
---

### 7.	Синхронизированный вывод
*Пояснение - Для модификации кода, чтобы вывод родительского и дочернего потока был синхронизирован, необходимо использовать mutex, который будет передавать право работы одному определённому потоку для последовательного вывода строк параллельных потоков.*


![image](https://github.com/user-attachments/assets/2665f643-af06-4b43-a326-8633bed51724)

---

### 8.	Перемножение квадратных матриц NxN
*Пояснение - Для выполнения операции перемножения матриц необходимо распределить на различные потоки определённое количество строк матрицы. В результате чего, на каждый поток ( за возможным исключением крайнего потока ), будет направлена одинаковая нагрузка в расчётах.*


![image](https://github.com/user-attachments/assets/5202b55c-087d-483d-a200-9aa402991364)

---

### 8.	Перемножение квадратных матриц NxN
*Пояснение - Для выполнения операции перемножения матриц необходимо распределить на различные потоки определённое количество строк матрицы. В результате чего, на каждый поток ( за возможным исключением крайнего потока ), будет направлена одинаковая нагрузка в расчётах.*

#### Пункт a, b, c.

![image](https://github.com/user-attachments/assets/6aa0b807-09ef-4c9e-a0be-dc55bb930427)

![image](https://github.com/user-attachments/assets/1b8a9aa3-5df7-42a2-86e7-1ee424c42514)

---

### 9.	Время выполнения
**Пояснение - Для реализации построения зависимости количества выделенных потоков на реализацию программы, создадим два дополнительных файла**

#### Файл для подсчёта времени выполнения

[Реализация](./pthread/counting.c)

```C
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
```
#### Файл для реализации построения графика ( реализован на языке программирования python )
*Пояснение - в списках приведены расчётные данные, полученный в результате выполнения программы counting.c *

[Реализация](./pthread/graph.py)

```python
import matplotlib.pyplot as plt

matrix_sizes = [100, 250, 500, 750, 1000, 1250, 1500]
time_1_thread = [11, 142, 1286, 7117, 17300, 30479, 37800]
time_2_threads = [10, 93, 602, 3007, 7718, 11989, 22072]
time_4_threads = [6, 49, 378, 2703, 4845, 9229, 16291]
time_8_threads = [5, 38, 285, 2110, 4464, 6906, 12232]
time_16_threads = [5, 36, 299, 1355, 3501, 6761, 10974]
time_32_threads = [6, 39, 303, 1262, 3092, 6581, 10770]
time_64_threads = [11, 38, 291, 1334, 3366, 6342, 11251]
time_128_threads = [13, 49, 287, 1242, 2878, 6611, 10937]


plt.figure(figsize=(10, 6))

plt.plot(matrix_sizes, time_1_thread, marker='', linestyle='-', color='blue', label='1')
plt.plot(matrix_sizes, time_2_threads, marker='', linestyle='-', color='red', label='2')
plt.plot(matrix_sizes, time_4_threads, marker='', linestyle='-', color='orange', label='4')
plt.plot(matrix_sizes, time_8_threads, marker='', linestyle='-', color='green', label='8')
plt.plot(matrix_sizes, time_16_threads, marker='', linestyle='-', color='purple', label='16')
plt.plot(matrix_sizes, time_32_threads, marker='', linestyle='-', color='brown', label='32')
plt.plot(matrix_sizes, time_64_threads, marker='', linestyle='-', color='yellow', label='64')
plt.plot(matrix_sizes, time_128_threads, marker='', linestyle='-', color='grey', label='128')

plt.xlabel('Размер матрицы')
plt.ylabel('Время (мс)') 
plt.title('Зависимость времени выполнения от размера матрицы и количества потоков')


plt.legend(title='Количество потоков')

plt.grid(True)

plt.tight_layout()
plt.show()

```
** Результатом вывода даннойпрограммы станет **

* (На изображении приведёнпример построения матриц 5x5 с 2 потоками)*


![image](https://github.com/user-attachments/assets/5ab19c2a-b2c1-4edd-8da3-0faa72c30c75)










