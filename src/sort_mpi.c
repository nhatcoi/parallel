#include "sort_ogt.h"
#include <string.h>

#ifdef HAVE_MPI
#include <mpi.h>
#include <sys/time.h>

// Định nghĩa cấu trúc để quản lý thông tin phân đoạn (chunk) trong MPI
// start: vị trí bắt đầu của chunk
// end: vị trí kết thúc của chunk
// size: kích thước của chunk
// rank: chỉ số của tiến trình
typedef struct {
    int start;
    int end;
    int size;
    int rank;
} MPIChunkInfo;

/**
 * Hàm trộn hai mảng con đã được sắp xếp thành một mảng đã sắp xếp
 * @param arr: Mảng cần trộn
 * @param left: Chỉ số bắt đầu của mảng con bên trái
 * @param mid: Chỉ số giữa
 * @param right: Chỉ số kết thúc của mảng con bên phải
 * @param ascending: 1 nếu sắp xếp tăng dần, 0 nếu sắp xếp giảm dần
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
 * Hàm trộn tất cả các phân đoạn đã sắp xếp từ các tiến trình MPI
 * @param arr: Mảng chứa tất cả các phân đoạn
 * @param chunk_sizes: Mảng chứa kích thước của từng phân đoạn
 * @param num_procs: Số lượng tiến trình
 * @param total_size: Tổng kích thước mảng
 * @param ascending: 1 nếu sắp xếp tăng dần, 0 nếu sắp xếp giảm dần
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
 * Hàm sắp xếp chèn song song sử dụng MPI
 * Triển khai thuật toán:
 * 1. Phân chia dữ liệu cho các tiến trình
 * 2. Mỗi tiến trình sắp xếp phần dữ liệu của mình
 * 3. Trộn các phần đã sắp xếp lại với nhau
 * 
 * @param a: Mảng cần sắp xếp
 * @param n: Kích thước mảng
 * @param ascending: 1 nếu sắp xếp tăng dần, 0 nếu sắp xếp giảm dần
 */
void parallelInsertionSortMPI(int a[], int n, int ascending) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (n <= 1) return;
    
    // Sử dụng sắp xếp tuần tự cho mảng nhỏ hoặc khi chỉ có một tiến trình
    // để tránh chi phí phụ trội của việc thiết lập MPI
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
    
    // Tính toán kích thước phân đoạn cho mỗi tiến trình để đảm bảo cân bằng tải
    int base_chunk_size = n / size;
    int remainder = n % size;
    
    // Tính toán kích thước phân đoạn cục bộ cho từng tiến trình
    // Các tiến trình đầu sẽ nhận thêm phần dư nếu có
    int local_chunk_size = base_chunk_size + (rank < remainder ? 1 : 0);
    
    // Khởi tạo mảng để lưu thông tin về kích thước và vị trí của các phân đoạn
    int* send_counts = NULL;
    int* displacements = NULL;
    
    // Chỉ tiến trình gốc (rank 0) cần cấp phát và tính toán thông tin phân phối
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
    
    // Cấp phát bộ nhớ cho mảng cục bộ của mỗi tiến trình
    int* local_array = (int*)malloc(local_chunk_size * sizeof(int));
    if (local_array == NULL) {
        printf(RED "Lỗi: Thất bại cấp phát bộ nhớ cho mảng cục bộ tại rank %d\n" RESET, rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Phân phối dữ liệu từ tiến trình gốc đến tất cả các tiến trình
    // Sử dụng MPI_Scatterv để hỗ trợ phân phối không đều
    MPI_Scatterv(a, send_counts, displacements, MPI_INT, 
                 local_array, local_chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Mỗi tiến trình độc lập sắp xếp phần dữ liệu của mình
    // Không cần đồng bộ hóa trong giai đoạn này
    if (ascending) {
        insertionSortAsc(local_array, local_chunk_size);
    } else {
        insertionSortDesc(local_array, local_chunk_size);
    }
    
    // Thu thập tất cả các phân đoạn đã sắp xếp về tiến trình gốc
    MPI_Gatherv(local_array, local_chunk_size, MPI_INT,
                a, send_counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Tiến trình gốc trộn tất cả các phân đoạn đã sắp xếp
    if (rank == 0) {
        merge_mpi_chunks(a, send_counts, size, n, ascending);
        free(send_counts);
        free(displacements);
    }
    
    // Đồng bộ hóa tất cả các tiến trình trước khi kết thúc
    MPI_Barrier(MPI_COMM_WORLD);
    
    free(local_array);
}

/**
 * Các hàm API công khai cho thuật toán sắp xếp chèn song song MPI
 * Cung cấp hai phiên bản: sắp xếp tăng dần và giảm dần
 */
void parallelInsertionSortMPIAsc(int a[], int n) {
    parallelInsertionSortMPI(a, n, 1);
}

void parallelInsertionSortMPIDesc(int a[], int n) {
    parallelInsertionSortMPI(a, n, 0);
}

/**
 * Khởi tạo môi trường MPI
 * @param argc Con trỏ đến số lượng tham số dòng lệnh
 * @param argv Con trỏ đến mảng các tham số dòng lệnh
 * @return 0 nếu thành công, -1 nếu thất bại
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
 * Kết thúc môi trường MPI
 * Giải phóng tài nguyên và đóng kết nối MPI
 */
void finalizeMPI(void) {
    MPI_Finalize();
}

/**
 * Lấy thông tin về tiến trình MPI hiện tại
 * @param rank Con trỏ để lưu chỉ số của tiến trình hiện tại
 * @param size Con trỏ để lưu tổng số tiến trình
 */
void getMPIInfo(int* rank, int* size) {
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

/**
 * Kiểm tra trạng thái khởi tạo của MPI
 * @return 1 nếu MPI đã được khởi tạo, 0 nếu chưa
 */
int isMPIInitialized(void) {
    int initialized;
    MPI_Initialized(&initialized);
    return initialized;
}

#else // HAVE_MPI not defined

// Stub implementations when MPI is not available
void parallelInsertionSortMPIAsc(int a[], int n) {
    printf(RED "MPI không khả dụng - chuyển sang sắp xếp tuần tự\n" RESET);
    insertionSortAsc(a, n);
}

void parallelInsertionSortMPIDesc(int a[], int n) {
    printf(RED "MPI không khả dụng - chuyển sang sắp xếp tuần tự\n" RESET);
    insertionSortDesc(a, n);
}

// Demonstration and benchmark stub functions moved to ogt_ui.c

int initializeMPI(int argc, char* argv[]) {
    printf(RED "Khởi tạo MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
    return -1;
}

void finalizeMPI(void) {
    printf(RED "Kết thúc MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
}

void getMPIInfo(int* rank, int* size) {
    *rank = 0;
    *size = 1;
    printf(RED "Thông tin MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
}

int isMPIInitialized(void) {
    return 0;
}

#endif // HAVE_MPI 