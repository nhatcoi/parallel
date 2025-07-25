#include "sort_ogt.h"
#include <string.h>

#ifdef HAVE_MPI
#include <mpi.h>
#include <sys/time.h>

// Struct chứa thông tin chunk trong MPI
typedef struct {
    int start;
    int end;
    int size;
    int rank;
} MPIChunkInfo;

/**
 * Trộn hai mảng đã sắp xếp (phiên bản MPI)
 */
void merge_two_arrays_mpi(int arr[], int left, int mid, int right, int ascending) {
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
 * Merges multiple sorted chunks from all MPI processes into a single sorted array in-place.
 *
 * Iteratively merges adjacent sorted chunks within the array using a binary merge approach,
 * updating chunk boundaries after each merge. Includes safety checks for invalid indices and
 * excessive iterations to prevent infinite loops. The merge order is determined by the
 * `ascending` flag.
 *
 * @param arr The array containing sorted chunks to be merged.
 * @param chunk_sizes Array specifying the size of each sorted chunk.
 * @param num_procs Number of MPI processes (number of chunks).
 * @param total_size Total number of elements in the array.
 * @param ascending If nonzero, merges in ascending order; otherwise, descending.
 */
void merge_mpi_chunks(int arr[], int* chunk_sizes, int num_procs, int total_size, int ascending) {
    if (num_procs <= 1) return;
    
    // Tính toán vị trí các chunk
    int* chunk_starts = (int*)malloc(num_procs * sizeof(int));
    chunk_starts[0] = 0;
    for (int i = 1; i < num_procs; i++) {
        chunk_starts[i] = chunk_starts[i-1] + chunk_sizes[i-1];
    }
    
    // Trộn các chunk một cách lặp lại sử dụng phương pháp trộn nhị phân
    int active_chunks = num_procs;
    int iteration = 0;
    const int MAX_ITERATIONS = 100; // Giới hạn an toàn
    
    while (active_chunks > 1) {
        // Kiểm tra an toàn để ngăn vòng lặp vô hạn
        iteration++;
        if (iteration > MAX_ITERATIONS) {
            printf(RED "Cảnh báo: MPI merge vượt quá số lần lặp tối đa, thoát\n" RESET);
            break;
        }
        
        int new_active = (active_chunks + 1) / 2;
        
        for (int i = 0; i < new_active; i++) {
            int left_idx = i * 2;
            int right_idx = left_idx + 1;
            
            if (right_idx < active_chunks) {
                // Trộn chunk left_idx và right_idx
                int left = chunk_starts[left_idx];
                int mid = chunk_starts[left_idx] + chunk_sizes[left_idx] - 1;
                int right = chunk_starts[right_idx] + chunk_sizes[right_idx] - 1;
                
                // Kiểm tra an toàn cho chỉ số hợp lệ
                if (left < 0 || mid >= total_size || right >= total_size || mid < left || right < mid) {
                    printf(RED "Cảnh báo: Phát hiện chỉ số merge không hợp lệ, bỏ qua\n" RESET);
                    continue;
                }
                
                merge_two_arrays_mpi(arr, left, mid, right, ascending);
                
                // Cập nhật thông tin chunk cho chunk đã trộn
                chunk_sizes[i] = chunk_sizes[left_idx] + chunk_sizes[right_idx];
                chunk_starts[i] = chunk_starts[left_idx];
            } else {
                // Số chunk lẻ, chuyển tiếp chunk cuối cùng
                chunk_sizes[i] = chunk_sizes[left_idx];
                chunk_starts[i] = chunk_starts[left_idx];
            }
        }
        
        // Kiểm tra an toàn: đảm bảo có tiến bộ TRƯỚC khi cập nhật active_chunks
        if (new_active >= active_chunks && active_chunks > 1) {
            printf(RED "Cảnh báo: MPI merge không có tiến bộ, thoát\n" RESET);
            break;
        }
        
        active_chunks = new_active;
    }
    
    free(chunk_starts);
}

/**
 * Performs a parallel insertion sort on an integer array using MPI.
 *
 * The array is divided into balanced chunks, distributed to all MPI processes, locally sorted, and then merged into a fully sorted array on the root process. Falls back to sequential insertion sort for small arrays or single-process execution.
 * @param a The array to be sorted. Only the root process's array is modified in-place.
 * @param n The number of elements in the array.
 * @param ascending Nonzero for ascending order, zero for descending order.
 */
void parallelInsertionSortMPI(int a[], int n, int ascending) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (n <= 1) return;
    
    // Đối với mảng nhỏ hoặc tiến trình đơn, dùng sắp xếp tuần tự để tránh overhead MPI
    if (n < 1000 || size <= 1) {
        if (rank == 0) {
            if (ascending) {
                insertionSortAsc(a, n);
            } else {
                insertionSortDesc(a, n);
            }
        }
        return;
    }
    
    // Tính toán kích thước chunk với cân bằng tải
    int base_chunk_size = n / size;
    int remainder = n % size;
    
    // Tính toán kích thước chunk cục bộ cho mỗi tiến trình
    int local_chunk_size = base_chunk_size + (rank < remainder ? 1 : 0);
    
    // Tính toán displacement cho scatterv
    int* send_counts = NULL;
    int* displacements = NULL;
    
    if (rank == 0) {
        send_counts = (int*)malloc(size * sizeof(int));
        displacements = (int*)malloc(size * sizeof(int));
        
        int current_pos = 0;
        for (int i = 0; i < size; i++) {
            send_counts[i] = base_chunk_size + (i < remainder ? 1 : 0);
            displacements[i] = current_pos;
            current_pos += send_counts[i];
        }
    }
    
    // Cấp phát mảng cục bộ
    int* local_array = (int*)malloc(local_chunk_size * sizeof(int));
    if (local_array == NULL) {
        printf(RED "Lỗi: Thất bại cấp phát bộ nhớ cho mảng cục bộ tại rank %d\n" RESET, rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Phân tán dữ liệu đến tất cả các tiến trình
    MPI_Scatterv(a, send_counts, displacements, MPI_INT, 
                 local_array, local_chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Sắp xếp chunk cục bộ (không cần đồng bộ - công việc độc lập)
    if (ascending) {
        insertionSortAsc(local_array, local_chunk_size);
    } else {
        insertionSortDesc(local_array, local_chunk_size);
    }
    
    // Gather sorted chunks back to root
    MPI_Gatherv(local_array, local_chunk_size, MPI_INT,
                a, send_counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Merge sorted chunks on root process
    if (rank == 0) {
        merge_mpi_chunks(a, send_counts, size, n, ascending);
        free(send_counts);
        free(displacements);
    }
    
    // Final synchronization
    MPI_Barrier(MPI_COMM_WORLD);
    
    free(local_array);
}

/**
 * Public API functions for MPI parallel insertion sort
 */
void parallelInsertionSortMPIAsc(int a[], int n) {
    parallelInsertionSortMPI(a, n, 1);
}

void parallelInsertionSortMPIDesc(int a[], int n) {
    parallelInsertionSortMPI(a, n, 0);
}

/**
 * Initializes the MPI environment with single-thread support.
 * @returns 0 on success, or -1 if MPI initialization fails.
 */
int initializeMPI(int argc, char* argv[]) {
    int provided;
    int result = MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    
    if (result != MPI_SUCCESS) {
        printf(RED "Lỗi khởi tạo MPI\n" RESET);
        return -1;
    }
    
    return 0;
}

/**
 * Finalize MPI environment
 */
void finalizeMPI(void) {
    MPI_Finalize();
}

/**
 * Get MPI process information
 */
void getMPIInfo(int* rank, int* size) {
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

/**
 * Benchmark function for MPI implementation
 */
// Benchmark and demonstration functions moved to ogt_ui.c to avoid duplication

/**
 * Utility function to check if MPI is properly initialized
 */
int isMPIInitialized(void) {
    int initialized;
    MPI_Initialized(&initialized);
    return initialized;
}

#else // HAVE_MPI not defined

/**
 * Sorts the array in ascending order using sequential insertion sort when MPI is unavailable.
 * Prints a warning indicating that MPI is not available and falls back to sequential sorting.
 */
void parallelInsertionSortMPIAsc(int a[], int n) {
    printf(RED "MPI không khả dụng - chuyển sang sắp xếp tuần tự\n" RESET);
    insertionSortAsc(a, n);
}

/**
 * Sorts an array in descending order using sequential insertion sort if MPI is unavailable.
 * Prints a warning message indicating that MPI is not enabled.
 */
void parallelInsertionSortMPIDesc(int a[], int n) {
    printf(RED "MPI không khả dụng - chuyển sang sắp xếp tuần tự\n" RESET);
    insertionSortDesc(a, n);
}

/**
 * Stub for MPI initialization when MPI is not compiled.
 * Always returns -1 to indicate MPI is unavailable.
 */

int initializeMPI(int argc, char* argv[]) {
    printf(RED "Khởi tạo MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
    return -1;
}

/**
 * Finalizes the MPI environment if available; otherwise, prints a warning that MPI is not compiled.
 */
void finalizeMPI(void) {
    printf(RED "Kết thúc MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
}

/**
 * Sets the MPI rank to 0 and size to 1, indicating that MPI is not available.
 */
void getMPIInfo(int* rank, int* size) {
    *rank = 0;
    *size = 1;
    printf(RED "Thông tin MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
}

/**
 * Indicates whether the MPI environment is initialized.
 * @returns 0, as MPI is not available in this build.
 */
int isMPIInitialized(void) {
    return 0;
}

#endif // HAVE_MPI 