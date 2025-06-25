#include "../../include/sort_ogt.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to print an array
static void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

// Simple demo function for basic insertion sort demonstration
void runBasicSortingDemo(void) {
    printf("\n" GREEN "=== MÔ TẢ THUẬT TOÁN SẮP XẾP CHÈN ===" RESET "\n");
    
    // Sample array for demo
    int arr[] = {12, 11, 13, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("\nOriginal array: ");
    printArray(arr, n);

    // Sequential insertion sort ascending
    insertionSortAsc(arr, n);
    printf("Array sorted in ascending order (sequential): ");
    printArray(arr, n);

    // Reset array for descending sort
    int arr2[] = {12, 11, 13, 5, 6, 7};

    // Sequential insertion sort descending
    insertionSortDesc(arr2, n);
    printf("Array sorted in descending order (sequential): ");
    printArray(arr2, n);
}

// Quick parallel demonstration
void runQuickParallelDemo(void) {
    printf("\n" CYAN "=== DEMO SONG SONG NHANH ===" RESET "\n");
    
    // Large array for parallel testing
    int large_size = 20000;
    int *large_arr = malloc(large_size * sizeof(int));
    int *parallel_arr = malloc(large_size * sizeof(int));
    
    // Generate random array
    srand(time(NULL));
    generateRandomArray(large_arr, large_size, 1000);
    copyArray(large_arr, parallel_arr, large_size);
    
    printf("Testing with array of %d elements...\n", large_size);
    
    // Sequential timing
    double start_time = getCurrentTime();
    insertionSortAsc(large_arr, large_size);
    double sequential_time = getCurrentTime() - start_time;
    
    // Parallel timing with 5 threads
    start_time = getCurrentTime();
    parallelInsertionSortAsc(parallel_arr, large_size, 5);
    double parallel_time = getCurrentTime() - start_time;
    
    printf("Sequential time: %.6f seconds\n", sequential_time);
    printf("Parallel time (5 threads): %.6f seconds\n", parallel_time);
    printf("Speedup: %.2fx\n", sequential_time / parallel_time);
    
    // Verify results
    int correct = 1;
    for (int i = 0; i < large_size; i++) {
        if (large_arr[i] != parallel_arr[i]) {
            correct = 0;
            break;
        }
    }
    printf("Results match: %s\n", correct ? "Yes ✓" : "No ✗");
    
    free(large_arr);
    free(parallel_arr);
} 