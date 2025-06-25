#include "../../include/sort_ogt.h"
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

// lấy thời gian hiện tại
double getCurrentTime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// cop mảng từ src sang dest
void copyArray(int src[], int dest[], int n) {
    memcpy(dest, src, n * sizeof(int));
}

// Tạo mảng random
void generateRandomArray(int arr[], int n, int max_val) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % max_val;
    }
} 