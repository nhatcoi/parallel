#include "sort_lib.h"

void insertionSortAsc(int a[], int n) {
    for (int i=1; i<n; i++) {
        int key = a[i];
        int j = i - 1;

        while (j >= 0 && a[j] > key) {
            a[j+1] = a[j];
            j--;
        }
        a[j+1] = key;
    }
}

void insertionSortDesc(int a[], int n) {
    for (int i=1; i<n; i++) {
        int key = a[i];
        int j = i - 1;

        while (j >= 0 && a[j] < key) {
            a[j+1] = a[j];
            j--;
        }
        a[j+1] = key;
    }
}
