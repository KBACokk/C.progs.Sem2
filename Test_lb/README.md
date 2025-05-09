# 📑 Тестирование ПО

* Краткое описание: В данной раоте было реализовано применение unit-тестов для проверки корректности работы программы. Реализация была осуществлена с помощью Фреймворка cmoka. *
  
![image](https://github.com/user-attachments/assets/4f739693-f447-4646-8ebe-eccd908e4e6e)


---

## 📚 Основное содержание файловой структуры
- ✅ Файл ( sort.c ) реализация проверяемых функций
- ✅ Файл ( sort.h ) объявление функций
- ✅ Файл ( test_sorts.c ) выполнение проверки с реализацией framework cmoka
- ✅ Файл ( CMakeLists.c ) выполняет сборку проекта 
---

## 🛠 Установка
```bash
git clone https://github.com/KBACokk/C_Programming_2_semestr/tree/main/<name_pack>
cd <репозиторий>
```
*Так же для работы проекта неоьходимо скачивание framework cmoka*
```bash
sudo apt-get update
sudo apt-get install libcmocka-dev
```
            
---

## 💡 Выполнение программы
```bash
mkdir build && cd build
cmake ..
make
ctest -V --output-on-failure
```

* Результатом вывода программы будет *

```bash
    Start 1: SortTests

1: Test command: /mnt/c/Users/<user>/Desktop/Test_lb/build/sort_tests
1: Test timeout computed to be: 10000000
1: [==========] Running 2 test(s).
1: [ RUN      ] test_insert_sort
1: [       OK ] test_insert_sort
1: [ RUN      ] test_fill_functions
1: [       OK ] test_fill_functions
1: [==========] 2 test(s) run.
1: [  PASSED  ] 2 test(s).
1/1 Test #1: SortTests ........................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.03 sec
```

⚠️ **Совет**:
*Если выполнение тестов произошло некоректно или не сработало вовсе, проверьте правильность установки cmoka, а так же попробуйте использовать различные эмуляторы linux (Например: wsl, VB Ubuntu)*


---

## Ссылки на все коды программы

- 📌 [Реализация](/Test_lb/sort.c)
- 📌 [Реализация](/Test_lb/sort.h)
- 📌 [Реализация](/Test_lb/test_sorts.c)
- 📌 [Реализация (Сборка проекта) ](/Test_lb/CMakeLists.txt)

---
