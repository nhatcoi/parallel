#include "../include/sort_lib.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>

// Original sequential insertion sort functions
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

// Parallel insertion sort using divide and conquer approach
void parallelInsertionSortAsc(int a[], int n, int num_threads) {
    omp_set_num_threads(num_threads);
    
    if (n <= 1) return;
    
    // For small arrays, use sequential sort
    if (n < 1000) {
        insertionSortAsc(a, n);
        return;
    }
    
    int chunk_size = n / num_threads;
    int **temp_arrays = malloc(num_threads * sizeof(int*));
    int *chunk_sizes = malloc(num_threads * sizeof(int));
    
    // Parallel sorting of chunks
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int start = thread_id * chunk_size;
        int end = (thread_id == num_threads - 1) ? n : start + chunk_size;
        int local_size = end - start;
        
        chunk_sizes[thread_id] = local_size;
        temp_arrays[thread_id] = malloc(local_size * sizeof(int));
        
        // Copy chunk to temporary array
        memcpy(temp_arrays[thread_id], &a[start], local_size * sizeof(int));
        
        // Sort the chunk using sequential insertion sort
        insertionSortAsc(temp_arrays[thread_id], local_size);
    }
    
    // Merge sorted chunks back into original array
    // This is a simple implementation - could be optimized further
    int *result = malloc(n * sizeof(int));
    int *indices = calloc(num_threads, sizeof(int));
    
    for (int i = 0; i < n; i++) {
        int min_val = INT_MAX;
        int min_thread = -1;
        
        // Find minimum value among all chunks
        for (int t = 0; t < num_threads; t++) {
            if (indices[t] < chunk_sizes[t] && temp_arrays[t][indices[t]] < min_val) {
                min_val = temp_arrays[t][indices[t]];
                min_thread = t;
            }
        }
        
        result[i] = min_val;
        indices[min_thread]++;
    }
    
    // Copy result back to original array
    memcpy(a, result, n * sizeof(int));
    
    // Cleanup
    for (int t = 0; t < num_threads; t++) {
        free(temp_arrays[t]);
    }
    free(temp_arrays);
    free(chunk_sizes);
    free(result);
    free(indices);
}

void parallelInsertionSortDesc(int a[], int n, int num_threads) {
    omp_set_num_threads(num_threads);
    
    if (n <= 1) return;
    
    // For small arrays, use sequential sort
    if (n < 1000) {
        insertionSortDesc(a, n);
        return;
    }
    
    int chunk_size = n / num_threads;
    int **temp_arrays = malloc(num_threads * sizeof(int*));
    int *chunk_sizes = malloc(num_threads * sizeof(int));
    
    // Parallel sorting of chunks
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int start = thread_id * chunk_size;
        int end = (thread_id == num_threads - 1) ? n : start + chunk_size;
        int local_size = end - start;
        
        chunk_sizes[thread_id] = local_size;
        temp_arrays[thread_id] = malloc(local_size * sizeof(int));
        
        // Copy chunk to temporary array
        memcpy(temp_arrays[thread_id], &a[start], local_size * sizeof(int));
        
        // Sort the chunk using sequential insertion sort
        insertionSortDesc(temp_arrays[thread_id], local_size);
    }
    
    // Merge sorted chunks back into original array (descending order)
    int *result = malloc(n * sizeof(int));
    int *indices = calloc(num_threads, sizeof(int));
    
    for (int i = 0; i < n; i++) {
        int max_val = INT_MIN;
        int max_thread = -1;
        
        // Find maximum value among all chunks
        for (int t = 0; t < num_threads; t++) {
            if (indices[t] < chunk_sizes[t] && temp_arrays[t][indices[t]] > max_val) {
                max_val = temp_arrays[t][indices[t]];
                max_thread = t;
            }
        }
        
        result[i] = max_val;
        indices[max_thread]++;
    }
    
    // Copy result back to original array
    memcpy(a, result, n * sizeof(int));
    
    // Cleanup
    for (int t = 0; t < num_threads; t++) {
        free(temp_arrays[t]);
    }
    free(temp_arrays);
    free(chunk_sizes);
    free(result);
    free(indices);
}

// Utility functions for benchmarking
double getCurrentTime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void copyArray(int src[], int dest[], int n) {
    memcpy(dest, src, n * sizeof(int));
}

void generateRandomArray(int arr[], int n, int max_val) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % max_val;
    }
} 