#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sort.h"

void FillInc(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = i + 1;
    }
}

void FillDec(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        A[i] = n - i;
    }
}

void FillRand(int n, int A[]) {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 100;
    }
}

int CheckSum(int n, int A[]) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += A[i];
    }
    return sum;
}

int RunNumber(int n, int A[]) {
    if (n == 0) return 0;
    int series = 1;
    for (int i = 1; i < n; i++) {
        if (A[i] < A[i - 1]) {
            series++;
        }
    }
    return series;
}

void PrintMas(int n, int A[]) {
    for (int i = 0; i < n; i++) {
        printf("%d ", A[i]);
    }
    printf("\n");
}

int insertSort(int n, int A[]) {
    int C = 0;
    int M = 0;

    for(int i = 1; i < n; i++) {
        int t = A[i]; 
        int j = i - 1;
        M++;

        while (j >= 0 && t < A[j]) {
            C++;
            A[j + 1] = A[j];
            M++;
            j--;
        }
        if (j >= 0) {
            C++;
        }
        
        A[j + 1] = t;
        M++;
    }
    return M + C;
}