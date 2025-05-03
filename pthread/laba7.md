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
![Создание потока]([https://via.placeholder.com/150x50?text=Project+Logo](https://sun9-74.userapi.com/s/v1/if2/oSisM-4NZtqmIV8a7hy2Lp7eypXGtbsUY2H4uT7lsNfJzyQr96qtXehEhe_Uff4zqY1Z2BIcGr9cabIrykrTtTl4.jpg?quality=95&as=32x30,48x46,72x68,108x102,160x152,240x228,360x341,480x455,540x512,640x607,720x683,817x775&from=bu&u=NY7-E6YrdtFTDQAP9Z9SimXLwAUao-MZnw9-thiXzDc&cs=604x573))

### 2.	Ожидание потока
*Пояснение - Изменим приоритет и обозначим, что дочерний поток должен выполнить свою программу, раньше основной “родительской программы”, для этого изменим pthread_join(thread, NULL) на более раннюю позицию, перед основной программой, что даст ей приоритет в выполнении.*
![redact1]([https://via.placeholder.com/150x50?text=Project+Logo](https://sun9-56.userapi.com/s/v1/if2/ZDUIcE4HXz8JKbdJ3PC3yWN0dgavLzJ7QOT4mQb1wKMiS_5VejPMhD3GwEmmg8nyqnS4jUtDrTb9seCnrczcxEUo.jpg?quality=95&as=32x29,48x44,72x66,108x99,160x147,240x221,360x331,480x442,540x497,640x589,720x662,799x735&from=bu&u=QO-ZDKRSylx3OHqXxugIzjEGHwqMKk2sXCLAaZ08XQY&cs=604x556)) 
