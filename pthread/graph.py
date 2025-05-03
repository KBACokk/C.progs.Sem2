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