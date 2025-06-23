#include <stdio.h>
#include "sort_lib.h"

// Function to print an array
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main(void) {
    int arr[] = {12, 11, 13, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Original array: ");
    printArray(arr, n);

    // Sort in ascending order
    insertionSortAsc(arr, n);
    printf("Array sorted in ascending order: ");
    printArray(arr, n);

    // Reset array
    int arr2[] = {12, 11, 13, 5, 6, 7};

    // Sort in descending order
    insertionSortDesc(arr2, n);
    printf("Array sorted in descending order: ");
    printArray(arr2, n);

    return 0;
}