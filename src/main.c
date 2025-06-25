#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/sort_lib.h"

// Function to print an array
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main(void) {
    printf("=== INSERTION SORT DEMONSTRATION ===\n\n");
    
    // Small array demonstration
    int arr[] = {12, 11, 13, 5, 6, 7};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Original array: ");
    printArray(arr, n);

    // Sort in ascending order
    insertionSortAsc(arr, n);
    printf("Array sorted in ascending order (sequential): ");
    printArray(arr, n);

    // Reset array
    int arr2[] = {12, 11, 13, 5, 6, 7};

    // Sort in descending order
    insertionSortDesc(arr2, n);
    printf("Array sorted in descending order (sequential): ");
    printArray(arr2, n);
    
    printf("\n=== PARALLEL SORTING DEMONSTRATION ===\n");
    
    // Larger array for parallel demonstration
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
    
    // Verify results are the same
    int correct = 1;
    for (int i = 0; i < large_size; i++) {
        if (large_arr[i] != parallel_arr[i]) {
            correct = 0;
            break;
        }
    }
    printf("Results match: %s\n", correct ? "Yes ✓" : "No ✗");
    
    printf("\n=== THREAD PERFORMANCE COMPARISON ===\n");
    printf("Thread Count | Time (seconds) | Speedup\n");
    printf("-------------|----------------|--------\n");
    
    int thread_counts[] = {1, 3, 5, 7, 9, 10, 11};
    int num_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    for (int i = 0; i < num_configs; i++) {
        int threads = thread_counts[i];
        copyArray(large_arr, parallel_arr, large_size);
        generateRandomArray(parallel_arr, large_size, 1000); // New random data
        
        start_time = getCurrentTime();
        if (threads == 1) {
            insertionSortAsc(parallel_arr, large_size);
        } else {
            parallelInsertionSortAsc(parallel_arr, large_size, threads);
        }
        double elapsed = getCurrentTime() - start_time;
        double speedup = (threads == 1) ? 1.0 : sequential_time / elapsed;
        
        printf("%-12d | %-14.6f | %.2fx\n", threads, elapsed, speedup);
    }
    
    printf("\nNote: For comprehensive benchmarking, run './prl_benchmark'\n");
    
    free(large_arr);
    free(parallel_arr);
    return 0;
} 