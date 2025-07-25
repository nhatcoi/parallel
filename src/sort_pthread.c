#include "sort_ogt.h"
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

// Struct chứa dữ liệu để truyền cho hàm thread
typedef struct {
    int* array;
    int start;
    int end;
    int thread_id;
    int ascending;
} ThreadData;

// Struct chứa thông tin về các phần tử của mảng
typedef struct {
    int start;
    int end;
    int size;
} ChunkInfo;

/**
 * Hàm thread để sắp xếp một chunk của mảng
 */
void* pthread_sort_chunk(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int chunk_size = data->end - data->start + 1;
    
    if (data->ascending) {
        insertionSortAsc(&data->array[data->start], chunk_size);
    } else {
        insertionSortDesc(&data->array[data->start], chunk_size);
    }
    
    return NULL;
}

/**
 * Merge 2 mảng đã sắp xếp
 */
void merge_two_arrays(int arr[], int left, int mid, int right, int ascending) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Tạo mảng tạm thời
    int* left_arr = (int*)malloc(n1 * sizeof(int));
    int* right_arr = (int*)malloc(n2 * sizeof(int));
    
    // Sao chép dữ liệu vào mảng tạm thời
    for (i = 0; i < n1; i++)
        left_arr[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        right_arr[j] = arr[mid + 1 + j];
    
    // Trộn các mảng tạm thời về arr[left..right]
    i = 0; j = 0; k = left;
    
    if (ascending) {
        while (i < n1 && j < n2) {
            if (left_arr[i] <= right_arr[j]) {
                arr[k] = left_arr[i];
                i++;
            } else {
                arr[k] = right_arr[j];
                j++;
            }
            k++;
        }
    } else {
        while (i < n1 && j < n2) {
            if (left_arr[i] >= right_arr[j]) {
                arr[k] = left_arr[i];
                i++;
            } else {
                arr[k] = right_arr[j];
                j++;
            }
            k++;
        }
        }
    
    // Sao chép các phần tử còn lại
    while (i < n1) {
        arr[k] = left_arr[i];
        i++; k++;
    }

    while (j < n2) {
        arr[k] = right_arr[j];
        j++; k++;
    }
    
    free(left_arr);
    free(right_arr);
}

/**
 * Merge các chunks đã sắp xếp bằng k-way merge
 */
void merge_sorted_chunks_pthread(int arr[], ChunkInfo chunks[], int num_threads, int n, int ascending) {
    // Trộn các khối một cách lặp lại
    while (num_threads > 1) {
        int new_num_threads = (num_threads + 1) / 2;
        
        for (int i = 0; i < new_num_threads; i++) {
            int left_idx = i * 2;
            int right_idx = left_idx + 1;
            
            if (right_idx < num_threads) {
                // Trộn chunks[left_idx] và chunks[right_idx]
                int left = chunks[left_idx].start;
                int mid = chunks[left_idx].end;
                int right = chunks[right_idx].end;
                
                merge_two_arrays(arr, left, mid, right, ascending);
                
                // Cập nhật thông tin chunk
                chunks[i].start = left;
                chunks[i].end = right;
                chunks[i].size = right - left + 1;
            } else {
                // Số khối lẻ, chuyển tiếp khối cuối cùng
                chunks[i] = chunks[left_idx];
            }
        }
        
        num_threads = new_num_threads;
    }
}

/**
 * Hàm triển khai pthread cốt lõi
 */
void parallelInsertionSortPthreads(int a[], int n, int num_threads, int ascending) {
    if (n <= 1) return;
    
    // Giới hạn số luồng
    if (num_threads > n) num_threads = n;
    if (num_threads > 32) num_threads = 32; // Practical limit
    
    // Tính chunksize
    int chunk_size = n / num_threads;
    int remainder = n % num_threads;
    
    // Tạo mảng thread và chunk info
    pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadData* thread_data = (ThreadData*)malloc(num_threads * sizeof(ThreadData));
    ChunkInfo* chunks = (ChunkInfo*)malloc(num_threads * sizeof(ChunkInfo));
    
    int current_pos = 0;
    
    // Setup dữ liệu thread và chunk
    for (int i = 0; i < num_threads; i++) {
        chunks[i].start = current_pos;
        chunks[i].size = chunk_size + (i < remainder ? 1 : 0);
        chunks[i].end = chunks[i].start + chunks[i].size - 1;
        
        thread_data[i].array = a;
        thread_data[i].start = chunks[i].start;
        thread_data[i].end = chunks[i].end;
        thread_data[i].thread_id = i;
        thread_data[i].ascending = ascending;
        
        current_pos += chunks[i].size;
    }
    
    // Tạo và run threads
    for (int i = 0; i < num_threads; i++) {
        int result = pthread_create(&threads[i], NULL, pthread_sort_chunk, &thread_data[i]);
        if (result != 0) {
            printf(RED "Lỗi tạo luồng %d: %d\n" RESET, i, result);
            exit(1);
        }
    }
    
    // Chờ các luồng hoàn thành
    for (int i = 0; i < num_threads; i++) {
        int result = pthread_join(threads[i], NULL);
        if (result != 0) {
            printf(RED "Lỗi join luồng %d: %d\n" RESET, i, result);
            exit(1);
        }
    }
    
    // Merge các chunks đã sắp xếp
    merge_sorted_chunks_pthread(a, chunks, num_threads, n, ascending);
    
    // Dọn dẹp
    free(threads);
    free(thread_data);
    free(chunks);
}

/**
 * Public API cho sắp xếp song song bằng Pthreads
 */
void parallelInsertionSortPthreadsAsc(int a[], int n, int num_threads) {
    parallelInsertionSortPthreads(a, n, num_threads, 1);
}

void parallelInsertionSortPthreadsDesc(int a[], int n, int num_threads) {
    parallelInsertionSortPthreads(a, n, num_threads, 0);
}
