# 📑 Реализация терминальной версии игры “Техасский холдем”

* Краткое описание: Данная работа реализована в формате расчётно-графическогой работы по предмету: "программирование" за 2 семестр *
![image](https://i.ytimg.com/vi/XYMO3RXUBS0/maxresdefault.jpg)
---

## 📚 Основное содержание файловой структуры
- ✅ Файл ( deck_operations.c ) Реализация всех функций для обработки структуры файлов колоды карт
- ✅ Файл ( game_logic.c ) Реализация всех функций отвечающих за игровую логику и реализацию карточных комбинаций
- ✅ Файл ( user_management.c ) Реализация функций игры для обработки взимодействия с пользователем ( регистрация, авторизация и т.д. )
- ✅ Файл ( main.c ) Основной файл с вызовом функций для реализации игрового процесса.
- ✅ Файл ( test_poker.c ) Объявление функций хэширования пароля и авторизацией пользователя.
- ✅ Файл ( testing.c ) реализация unit-тестов для проверки корректности работы функций на основе фреймворка cmocka
- ✅ Файл ( CMakeLists.c ) выполняет сборку проекта 
---

## 🛠 Установка
```bash
git clone https://github.com/KBACokk/C_Programming_2_semestr/tree/main/<name_pack>
cd <репозиторий>
```
*Так же для работы проекта неоьходимо скачивание framework cmocka*
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
./game_play  
ctest -V --output-on-failure
```

---

## Ссылки на все коды программы
- 📌 [Реализация](/RGRProg/deck_operations.c)
- 📌 [Реализация](/RGRProg/game_logic.c)
- 📌 [Реализация](/RGRProg/user_management.c)
- 📌 [Реализация](/RGRProg/main.c)
- 📌 [Реализация](/RGRProg/test_poker.c)
- 📌 [Реализация](/RGRProg/testing.c)
- 📌 [Реализация](/RGRProg/CMakeLists.c)






