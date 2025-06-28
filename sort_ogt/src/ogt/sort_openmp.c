#include "../../include/sort_ogt.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Parallel insertion sort - ascending order (manual chunking approach)
void parallelInsertionSortAsc(int a[], int n, int num_threads) {
    omp_set_num_threads(num_threads); // set số thread
    
    if (n <= 1) return; // nếu n <= 1 thì return
    
    // dùng tuần tự cho kích thước bé
    if (n < 1000) {
        insertionSortAsc(a, n);
        return;
    }
    
    // Pre-allocate memory để tránh overhead trong parallel region
    int chunk_size = n / num_threads;
    int **temp_arrays = malloc(num_threads * sizeof(int*));
    int *chunk_sizes = malloc(num_threads * sizeof(int));
    
    // Pre-allocate tất cả temp arrays trước parallel region
    for (int t = 0; t < num_threads; t++) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? n : start + chunk_size;
        chunk_sizes[t] = end - start;
        temp_arrays[t] = malloc(chunk_sizes[t] * sizeof(int));
    }
    
    // Single parallel region - hiệu quả hơn parallel for
    #pragma omp parallel
    {
        int tid = omp_get_thread_num(); // lấy id của thread hiện tại
        int start = tid * chunk_size;
        int local_size = chunk_sizes[tid]; // size mỗi thread
        
        // copy phần tử của mỗi thread vào temp_arrays
        memcpy(temp_arrays[tid], &a[start], local_size * sizeof(int));
        
        // sort mỗi thread
        insertionSortAsc(temp_arrays[tid], local_size);
    }
    
    // k-way merge của sorted chunks
    int *result = malloc(n * sizeof(int));
    int *indices = calloc(num_threads, sizeof(int));
    
    for (int i = 0; i < n; i++) {
        int min_val = INT_MAX;
        int min_thread = -1;
        
        // tìm phần tử nhỏ nhất trong tất cả chunks
        for (int t = 0; t < num_threads; t++) {
            if (indices[t] < chunk_sizes[t] && temp_arrays[t][indices[t]] < min_val) {
                min_val = temp_arrays[t][indices[t]];
                min_thread = t;
            }
        }
        
        result[i] = min_val;
        indices[min_thread]++;
    }
    
    // copy result vào a
    memcpy(a, result, n * sizeof(int));
    
    // giải phóng bộ nhớ
    for (int t = 0; t < num_threads; t++) {
        free(temp_arrays[t]);
    }
    free(temp_arrays);
    free(chunk_sizes);
    free(result);
    free(indices);
}

// Parallel insertion sort - descending order
void parallelInsertionSortDesc(int a[], int n, int num_threads) {
    omp_set_num_threads(num_threads);
    
    if (n <= 1) return;
    
    // dùng tuần tự cho kích thước bé
    if (n < 1000) {
        insertionSortDesc(a, n);
        return;
    }
    
    // Manual chunking approach (hiệu quả hơn parallel for)
    int chunk_size = n / num_threads;
    int **temp_arrays = malloc(num_threads * sizeof(int*));
    int *chunk_sizes = malloc(num_threads * sizeof(int));
    
    // Pre-allocate memory
    for (int t = 0; t < num_threads; t++) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? n : start + chunk_size;
        chunk_sizes[t] = end - start;
        temp_arrays[t] = malloc(chunk_sizes[t] * sizeof(int));
    }
    
    // Single parallel region
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk_size;
        int local_size = chunk_sizes[tid];
        
        // copy phần tử của chunk này vào temp_arrays
        memcpy(temp_arrays[tid], &a[start], local_size * sizeof(int));
        
        // sort chunk này
        insertionSortDesc(temp_arrays[tid], local_size);
    }
    
    // k-way merge của sorted chunks (descending)
    int *result = malloc(n * sizeof(int));
    int *indices = calloc(num_threads, sizeof(int));
    
    for (int i = 0; i < n; i++) {
        int max_val = INT_MIN;
        int max_thread = -1;
        
        // tìm phần tử lớn nhất trong tất cả chunks
        for (int t = 0; t < num_threads; t++) {
            if (indices[t] < chunk_sizes[t] && temp_arrays[t][indices[t]] > max_val) {
                max_val = temp_arrays[t][indices[t]];
                max_thread = t;
            }
        }
        
        result[i] = max_val;
        indices[max_thread]++;
    }
    
    // copy result vào a
    memcpy(a, result, n * sizeof(int));
    
    // giải phóng bộ nhớ
    for (int t = 0; t < num_threads; t++) {
        free(temp_arrays[t]);
    }
    free(temp_arrays);
    free(chunk_sizes);
    free(result);
    free(indices);
} 