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
// Ranodm comment
// Second rand comment