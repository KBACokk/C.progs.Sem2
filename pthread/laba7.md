# 🚀 Название проекта 

**Краткое описание**: В данном проекте показано освоение темы многопоточности, реализованной на языке программирования C.

---

## 🔥 Основные пункты 
- ✅ Реализация работы на 60 баллов ( оценка 3 )
- ✅ Реализация работы на 80 баллов ( оценка 4 )

---

## 💻 Знакомство с pthread:
---
### 1.	Создать поток
```C
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MESSAGES 5

void* thread_func(void* arg) {
    for (int i = 0; i < MESSAGES; i++) {
        printf ("Дочерний поток: строка %d\n", i);
    }
   return NULL; 
}

int main () {
    pthread_t thread;

    pthread_create(&thread, NULL, thread_func, NULL);

    for (int i = 0; i < MESSAGES; i++) {
        printf ("Родительский поток: строка %d\n", i);
    }

    pthread_join(thread, NULL);
}
