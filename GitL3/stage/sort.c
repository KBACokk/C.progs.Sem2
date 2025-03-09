#include <stdio.h>

int InsertingSort(int arr[], int n) {
    int T = 0, C = 0, M = 0, temp = 0;
    for (int i = 1; i < n; i++) {
        M++; temp = arr[i];
        int j = i - 1;
        while (j >= 0 && temp < arr[j]) {
            M++; arr[j + 1] = arr[j];
            C++;
            j--;
        }
        if (j >= 0) {
            C++;
        }
        M++; arr[j + 1] = temp;
    }
    return T = M + C;
}

 int main () {
	int b = 3;
	int a[] = {4,0,2};
	InsertingSort(a, 3);
    for (int i = 0; i < b; i++) {
        printf ("%d\n", a[i]);
    }
 }