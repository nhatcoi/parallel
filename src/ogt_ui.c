#include "sort_ogt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  // cho isatty()
#include <omp.h>     // cho các hàm OpenMP

#define MAX_ARRAY_SIZE 100000
#define MAX_VALUE 10000
#define NUM_RUNS 5

// ========== CÁC HÀM TIỆN ÍCH ==========

// Hàm in mảng
static void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

// Hàm lấy số luồng từ người dùng với xác thực
static int getThreadCountInput(void) {
    int threads;
    int max_threads = omp_get_max_threads();
    
    // Hiển thị thông tin MPI nếu có
#ifdef HAVE_MPI
    int rank = 0, size = 1;
    if (isMPIInitialized()) {
        getMPIInfo(&rank, &size);
        printf("\n" CYAN "📏 Số tiến trình MPI hiện tại: %d" RESET "\n", size);
    }
#endif
    
    printf("\n" CYAN "📏 Nhập số luồng cho OpenMP và Pthreads (1-%d): " RESET, max_threads);
    fflush(stdout);  // Force flush buffer để hiển thị prompt
    scanf("%d", &threads);
    
    // Clear input buffer sau khi đọc để tránh ảnh hưởng lần sau
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // Xác thực đầu vào
    if (threads < 1) {
        printf(YELLOW "⚠️  Số luồng quá nhỏ, sử dụng 1 luồng" RESET "\n");
        threads = 1;
    } else if (threads > max_threads) {
        printf(YELLOW "⚠️  Số luồng quá lớn, sử dụng %d luồng (max)" RESET "\n", max_threads);
        threads = max_threads;
    }
    
    printf(GREEN "✅ Sử dụng %d luồng" RESET "\n", threads);
    return threads;
}

// Hàm lấy kích thước mảng từ người dùng với xác thực
static int getArraySizeInput(void) {
    int array_size;
    const int MIN_SIZE = 1000;
    const int MAX_SIZE = 1000000;
    
    printf("\n" CYAN "📏 Nhập số phần tử mảng (%d-%d): " RESET, MIN_SIZE, MAX_SIZE);
    fflush(stdout);  // Force flush buffer để hiển thị prompt
    scanf("%d", &array_size);
    
    // Clear input buffer sau khi đọc để tránh ảnh hưởng lần sau
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    // Xác thực đầu vào
    if (array_size < MIN_SIZE) {
        printf(YELLOW "⚠️  Kích thước quá nhỏ, sử dụng %d phần tử" RESET "\n", MIN_SIZE);
        array_size = MIN_SIZE;
    } else if (array_size > MAX_SIZE) {
        printf(YELLOW "⚠️  Kích thước quá lớn, sử dụng %d phần tử" RESET "\n", MAX_SIZE);
        array_size = MAX_SIZE;
    }
    
    printf(GREEN "✅ Sử dụng %d phần tử" RESET "\n", array_size);
    return array_size;
}

// In kết quả benchmark theo bảng định dạng
void printBenchmarkResults(const char* sort_type, int array_size, int threads, double avg_time, double speedup) {
    printf("%-15s | %-10d | %-8d | %-12.6f | %-10.4f\n", 
           sort_type, array_size, threads, avg_time, speedup);
}

// In thông tin thư viện
void printLibraryInfo(void) {
    printf("\n" MAGENTA "╔══════════════════════════════════════════════════════╗\n");
    printf("║                   THƯ VIỆN SORT OGT                  ║\n");
    printf("║                Sắp Xếp Chèn Song Song                ║\n");
    printf("║                   Phiên bản: %-8s                ║\n", SORT_OGT_VERSION);
    printf("║                   Tác giả: %-9s                 ║\n", SORT_OGT_AUTHOR);
    printf("╚══════════════════════════════════════════════════════╝" RESET "\n");
}

// In thông tin hệ thống
void printSystemInformation(void) {
    printf("\n" YELLOW "=== THÔNG TIN HỆ THỐNG ===" RESET "\n");
    printf("Số luồng tối đa: %d\n", omp_get_max_threads());
    printf("Số tiến trình: %d\n", omp_get_num_procs());
    printf("Phiên bản OpenMP: %d\n", _OPENMP);
    printf("Phiên bản thư viện: %s\n", SORT_OGT_VERSION);
    printf("Tác giả: %s\n", SORT_OGT_AUTHOR);
    printf("Cấu hình Test:\n");
    printf("- Kích thước mảng: 10K, 25K, 50K, 75K, 100K phần tử\n");
    printf("- Số luồng: 1, 3, 5, 7, 9, 11\n");
    printf("- Số lần chạy mỗi cấu hình: %d\n", NUM_RUNS);
    printf("- Giá trị ngẫu nhiên tối đa: %d\n", MAX_VALUE);
    printf("\n");
}

// ========== 1. CÁC HÀM SẮP XẾP TUẦN TỰ ==========

void runSequentialDemo(void) {
    printf("\n" GREEN "=== DEMO TUẦN TỰ (SEQUENTIAL) ===" RESET "\n");
    
    // Mẫu mảng
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Mảng ban đầu: ");
    printArray(arr, n);

    // Test ascending sort
    int *test_arr = malloc(n * sizeof(int));
    copyArray(arr, test_arr, n);
    
    double start_time = getCurrentTime();
    insertionSortAsc(test_arr, n);
    double end_time = getCurrentTime();
    
    printf("Sắp xếp tăng dần: ");
    printArray(test_arr, n);
    printf("Thời gian: %.6f giây\n", end_time - start_time);

    // Test descending sort
    copyArray(arr, test_arr, n);
    start_time = getCurrentTime();
    insertionSortDesc(test_arr, n);
    end_time = getCurrentTime();
    
    printf("Sắp xếp giảm dần: ");
    printArray(test_arr, n);
    printf("Thời gian: %.6f giây\n", end_time - start_time);
    
    free(test_arr);
}

void runSequentialBenchmark(void) {
    printf("\n" GREEN "=== BENCHMARK TUẦN TỰ (SEQUENTIAL) ===" RESET "\n");
    
    int test_sizes[] = {1000, 5000, 10000, 25000, 50000};
    int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    printf("%-12s | %-12s\n", "Kích Thước", "Thời Gian TB (s)");
    printf("------------------------\n");
    
    for (int i = 0; i < num_sizes; i++) {
        int size = test_sizes[i];
        double total_time = 0.0;
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(size * sizeof(int));
            generateRandomArray(arr, size, MAX_VALUE);
            
            double start_time = getCurrentTime();
            insertionSortAsc(arr, size);
            double end_time = getCurrentTime();
            
            total_time += (end_time - start_time);
            free(arr);
        }
        
        double avg_time = total_time / NUM_RUNS;
        printf("%-12d | %-12.6f\n", size, avg_time);
    }
}

// ========== 2. CÁC HÀM SẮP XẾP OPENMP ==========

void runOpenMPDemo(void) {
    printf("\n" CYAN "=== DEMO OPENMP ===" RESET "\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 72, 17, 95, 33, 47, 61, 8, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    int threads = getThreadCountInput();

    printf("Mảng ban đầu: ");
    printArray(arr, n);
    printf("Sử dụng %d luồng\n", threads);

    // Test ascending sort
    int *test_arr = malloc(n * sizeof(int));
    copyArray(arr, test_arr, n);
    
    double start_time = getCurrentTime();
    parallelInsertionSortAsc(test_arr, n, threads);
    double end_time = getCurrentTime();
    
    printf("Sắp xếp tăng dần (OpenMP): ");
    printArray(test_arr, n);
    printf("Thời gian: %.6f giây\n", end_time - start_time);
    
    free(test_arr);
}

void runOpenMPBenchmark(void) {
    printf("\n" CYAN "=== BENCHMARK OPENMP ===" RESET "\n");
    
    // Get array size from user
    int array_size = getArraySizeInput();
    
    // Fixed thread counts: p = 1 (sequential), 3, 5, 7, 9, 11
    int thread_counts[] = {1, 3, 5, 7, 9, 11};
    int num_thread_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("\n" MAGENTA "🔥 BENCHMARK VỚI THREADS CỐ ĐỊNH (p=1,3,5,7,9,11)" RESET "\n");
    printf("Kích thước mảng: %d phần tử\n", array_size);
    printf("Số lần chạy mỗi cấu hình: %d\n\n", NUM_RUNS);
    
    printf("%-8s | %-12s | %-10s | %-12s\n", "Luồng", "Thời Gian TB (s)", "Tăng Tốc", "Hiệu Suất");
    printf("----------------------------------------------------\n");
    
    double sequential_time = 0.0;
    
    for (int t = 0; t < num_thread_configs; t++) {
        int threads = thread_counts[t];
        double total_time = 0.0;
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            if (!arr) {
                printf(RED "❌ Cấp phát bộ nhớ thất bại cho %d phần tử\n" RESET, array_size);
                return;
            }
            
            generateRandomArray(arr, array_size, MAX_VALUE);
            
            double start_time = getCurrentTime();
            
            if (threads == 1) {
                insertionSortAsc(arr, array_size);
            } else {
                parallelInsertionSortAsc(arr, array_size, threads);
            }
            
            double end_time = getCurrentTime();
            total_time += (end_time - start_time);
            free(arr);
        }
        
        double avg_time = total_time / NUM_RUNS;
        
        if (threads == 1) {
            sequential_time = avg_time;
            printf("%-8d | %-12.6f | %-10.2f | %-12.2f%%\n", 
                   threads, avg_time, 1.0, 100.0);
        } else {
            double speedup = sequential_time / avg_time;
            double efficiency = (speedup / threads) * 100.0;
            printf("%-8d | %-12.6f | %-10.2f | %-12.2f%%\n", 
                   threads, avg_time, speedup, efficiency);
        }
    }
    
    printf("\n" CYAN "=== PHÂN TÍCH KẾT QUẢ ===" RESET "\n");
    printf("✅ Sắp xếp tuần tự: %.6f s\n", sequential_time);
    printf("🎯 Các số luồng test: 1(tuần tự), 3, 5, 7, 9, 11\n");
    printf("📊 Kích thước mảng: %d phần tử\n", array_size);
    printf("📈 Hiệu suất = (Tăng tốc / Số luồng) × 100%%\n");
}

// ========== 3. CÁC HÀM SẮP XẾP PTHREADS ==========

void runPthreadsDemo(void) {
    printf("\n" BLUE "=== DEMO PTHREADS ===" RESET "\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 72, 17, 95, 33, 47, 61, 8, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    int threads = getThreadCountInput();

    printf("Mảng ban đầu: ");
    printArray(arr, n);
    printf("Sử dụng %d luồng\n", threads);

    // Test ascending sort
    int *test_arr = malloc(n * sizeof(int));
    copyArray(arr, test_arr, n);
    
    double start_time = getCurrentTime();
    parallelInsertionSortPthreadsAsc(test_arr, n, threads);
    double end_time = getCurrentTime();
    
    printf("Sắp xếp tăng dần (Pthreads): ");
    printArray(test_arr, n);
    printf("Thời gian: %.6f giây\n", end_time - start_time);
    
    // Verify correctness
    int is_sorted = 1;
    for (int i = 1; i < n; i++) {
        if (test_arr[i] < test_arr[i-1]) {
            is_sorted = 0;
            break;
        }
    }
    printf("Tính đúng đắn: %s\n", is_sorted ? "✅ ĐẠT" : "❌ THẤT BẠI");
    
    free(test_arr);
}

void runPthreadsBenchmark(void) {
    printf("\n" BLUE "=== BENCHMARK PTHREADS ===" RESET "\n");
    
    // Get array size from user
    int array_size = getArraySizeInput();
    
    // Fixed thread counts: p = 1 (sequential), 3, 5, 7, 9, 11
    int thread_counts[] = {1, 3, 5, 7, 9, 11};
    int num_thread_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("\n" MAGENTA "🔥 BENCHMARK VỚI THREADS CỐ ĐỊNH (p=1,3,5,7,9,11)" RESET "\n");
    printf("Kích thước mảng: %d phần tử\n", array_size);
    printf("Số lần chạy mỗi cấu hình: %d\n\n", NUM_RUNS);
    
    printf("%-8s | %-12s | %-10s | %-12s\n", "Luồng", "Thời Gian TB (s)", "Tăng Tốc", "Hiệu Suất");
    printf("----------------------------------------------------\n");
    
    double sequential_time = 0.0;
    
    for (int t = 0; t < num_thread_configs; t++) {
        int threads = thread_counts[t];
        double total_time = 0.0;
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            if (!arr) {
                printf(RED "❌ Cấp phát bộ nhớ thất bại cho %d phần tử\n" RESET, array_size);
                return;
            }
            
            generateRandomArray(arr, array_size, MAX_VALUE);
            
            double start_time = getCurrentTime();
            
            if (threads == 1) {
                insertionSortAsc(arr, array_size);
            } else {
                parallelInsertionSortPthreadsAsc(arr, array_size, threads);
            }
            
            double end_time = getCurrentTime();
            total_time += (end_time - start_time);
            free(arr);
        }
        
        double avg_time = total_time / NUM_RUNS;
        
        if (threads == 1) {
            sequential_time = avg_time;
            printf("%-8d | %-12.6f | %-10.2f | %-12.2f%%\n", 
                   threads, avg_time, 1.0, 100.0);
        } else {
            double speedup = sequential_time / avg_time;
            double efficiency = (speedup / threads) * 100.0;
            printf("%-8d | %-12.6f | %-10.2f | %-12.2f%%\n", 
                   threads, avg_time, speedup, efficiency);
        }
    }
    
    printf("\n" CYAN "=== PHÂN TÍCH KẾT QUẢ ===" RESET "\n");
    printf("✅ Chuẩn tuần tự: %.6f giây\n", sequential_time);
    printf("🎯 Số luồng đã test: 1(tuần tự), 3, 5, 7, 9, 11\n");
    printf("📊 Kích thước mảng: %d phần tử\n", array_size);
    printf("📈 Hiệu suất = (Tăng tốc / Số luồng) × 100%%\n");
}

// ========== 4. CÁC HÀM SẮP XẾP MPI ==========

void runMPIDemo(void) {
    printf("\n" YELLOW "=== DEMO MPI ===" RESET "\n");
    
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("Chạy demo MPI với %d tiến trình\n", size);
    }
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 72, 17, 95, 33, 47, 61, 8, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    if (rank == 0) {
        printf("Mảng ban đầu: ");
        printArray(arr, n);
    }
    
    double start_time = getCurrentTime();
    parallelInsertionSortMPIAsc(arr, n);
    double end_time = getCurrentTime();
    
    if (rank == 0) {
        printf("Sắp xếp tăng dần (MPI): ");
        printArray(arr, n);
        printf("Thời gian: %.6f giây\n", end_time - start_time);
    }
#else
    printf("MPI không khả dụng - hiển thị fallback sang sắp xếp tuần tự\n");
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Mảng ban đầu: ");
    printArray(arr, n);
    
    double start_time = getCurrentTime();
    parallelInsertionSortMPIAsc(arr, n);  // Will fallback to sequential
    double end_time = getCurrentTime();
    
    printf("Đã sắp xếp (fallback): ");
    printArray(arr, n);
    printf("Thời gian: %.6f giây\n", end_time - start_time);
#endif
}

void runComprehensiveMPITest(void) {
    runMPIBenchmark();
}

void runMPIBenchmark(void) {
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("\n" YELLOW "=== BENCHMARK MPI ===" RESET "\n");
    }
    
    int array_size;
    if (rank == 0) {
        printf("Số tiến trình MPI hiện tại: %d\n", size);
        array_size = getArraySizeInput();
        
        printf("\n" MAGENTA "🔥 BENCHMARK VỚI TIẾN TRÌNH CỐ ĐỊNH (p=%d)" RESET "\n", size);
        printf("Kích thước mảng: %d phần tử\n", array_size);
        printf("Số lần chạy mỗi cấu hình: %d\n\n", NUM_RUNS);
        
        
        printf("%-10s | %-12s | %-10s | %-12s\n", "Tiến Trình", "Thời Gian TB (s)", "Tăng Tốc", "Hiệu Suất");
        printf("-------------------------------------------------------\n");
    }
    
    // Broadcast array size to all processes
    MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Get sequential baseline (only on rank 0)
    double sequential_time = 0.0;
    if (rank == 0) {
        double total_seq_time = 0.0;
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            generateRandomArray(arr, array_size, MAX_VALUE);
            
            double start_time = getCurrentTime();
            insertionSortAsc(arr, array_size);
            double end_time = getCurrentTime();
            
            total_seq_time += (end_time - start_time);
            free(arr);
        }
        sequential_time = total_seq_time / NUM_RUNS;
    }
    
    // Broadcast sequential time to all processes
    MPI_Bcast(&sequential_time, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // MPI benchmark
    double total_mpi_time = 0.0;
    for (int run = 0; run < NUM_RUNS; run++) {
        int *arr = NULL;
        
        if (rank == 0) {
            arr = malloc(array_size * sizeof(int));
            generateRandomArray(arr, array_size, MAX_VALUE);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = getCurrentTime();
        parallelInsertionSortMPIAsc(arr, array_size);
        double end_time = getCurrentTime();
        
        if (rank == 0) {
            total_mpi_time += (end_time - start_time);
            free(arr);
        }
    }
    
    if (rank == 0) {
        double avg_mpi_time = total_mpi_time / NUM_RUNS;
        double speedup = sequential_time / avg_mpi_time;
        double efficiency = (speedup / size) * 100.0;
        
        printf("%-10d | %-12.6f | %-10.2f | %-12.2f%%\n", 
               size, avg_mpi_time, speedup, efficiency);
        
        printf("\n" CYAN "=== PHÂN TÍCH KẾT QUẢ ===" RESET "\n");
        printf("✅ Chuẩn tuần tự: %.6f giây\n", sequential_time);
        printf("🚀 MPI (%d processes): %.6f giây\n", size, avg_mpi_time);
        printf("⚡ Tăng tốc: %.2fx\n", speedup);
        printf("🎯 Số process hiện tại: %d\n", size);
        printf("📊 Kích thước mảng: %d phần tử\n", array_size);
        printf("📈 Hiệu suất = (Tăng tốc / Số tiến trình) × 100%%\n");
        
        if (efficiency > 100.0) {
            printf("🚀 Phát hiện tăng tốc siêu tuyến tính! (hiệu ứng cache hoặc lợi ích thuật toán)\n");
        } else if (efficiency > 70.0) {
            printf("✅ Hiệu suất song song tốt\n");
        } else {
            printf("⚠️  Hiệu suất thấp - xem xét overhead giao tiếp\n");
        }
        
    }
    
#else
    printf("\n" YELLOW "=== BENCHMARK MPI ===" RESET "\n");
    printf("MPI không khả dụng - không thể chạy benchmark MPI\n");
    printf("Điều này sẽ hiển thị so sánh giữa MPI và sắp xếp tuần tự\n");
#endif
}

// ========== 5. HÀM SO SÁNH ==========

void runAllComparison(void) {
    // MPI info
    int rank = 0, size = 1;
#ifdef HAVE_MPI
    if (isMPIInitialized()) {
        getMPIInfo(&rank, &size);
    }
#endif
    
    int array_size, threads;
    
    // Only rank 0 gets user input
    if (rank == 0) {
        printf("\n" MAGENTA "=== SO SÁNH TẤT CẢ 4 KIỂU SORT ===" RESET "\n");
        
        // Get array size from user
        array_size = getArraySizeInput();
        
        // Get thread count from user  
        threads = getThreadCountInput();
        
        printf("\n" CYAN "🔄 Bắt đầu so sánh với các thông số:" RESET "\n");
        printf("Kích thước mảng: %d phần tử\n", array_size);
        printf("Số luồng (cho phương pháp song song): %d\n", threads);
        printf("Số lần chạy mỗi phương pháp: %d\n\n", NUM_RUNS);
        printf("%-15s | %-12s | %-10s\n", "Phương Pháp", "Thời Gian TB (s)", "Tăng Tốc");
        printf("------------------------------------------\n");
    }
    
#ifdef HAVE_MPI
    // Broadcast user input to all MPI processes
    if (isMPIInitialized()) {
        MPI_Bcast(&array_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&threads, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
#endif
    
    double times[4] = {0, 0, 0, 0};
    const char* methods[] = {"Tuần Tự", "OpenMP", "Pthreads", "MPI"};
    
    // Only rank 0 runs sequential, OpenMP, and Pthreads tests
    if (rank == 0) {
        // Generate test data
        int *original = malloc(array_size * sizeof(int));
        generateRandomArray(original, array_size, MAX_VALUE);
        
        // 1. Sequential
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            copyArray(original, arr, array_size);
            
            double start_time = getCurrentTime();
            insertionSortAsc(arr, array_size);
            double end_time = getCurrentTime();
            
            times[0] += (end_time - start_time);
            free(arr);
        }
        times[0] /= NUM_RUNS;
        
        // 2. OpenMP
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            copyArray(original, arr, array_size);
            
            double start_time = getCurrentTime();
            parallelInsertionSortAsc(arr, array_size, threads);
            double end_time = getCurrentTime();
            
            times[1] += (end_time - start_time);
            free(arr);
        }
        times[1] /= NUM_RUNS;
        
        // 3. Pthreads
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            copyArray(original, arr, array_size);
            
            double start_time = getCurrentTime();
            parallelInsertionSortPthreadsAsc(arr, array_size, threads);
            double end_time = getCurrentTime();
            
            times[2] += (end_time - start_time);
            free(arr);
        }
        times[2] /= NUM_RUNS;
        
        free(original);
    }
    
    // 4. MPI - All processes participate
#ifdef HAVE_MPI
    if (isMPIInitialized()) {
        // Broadcast first 3 results to all processes for consistency
        MPI_Bcast(times, 3, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = NULL;
            if (rank == 0) {
                arr = malloc(array_size * sizeof(int));
                generateRandomArray(arr, array_size, MAX_VALUE);
            }
            
            MPI_Barrier(MPI_COMM_WORLD);
            double start_time = getCurrentTime();
            parallelInsertionSortMPIAsc(arr, array_size);
            double end_time = getCurrentTime();
            
            if (rank == 0) {
                times[3] += (end_time - start_time);
                free(arr);
            }
        }
        
        if (rank == 0) {
            times[3] /= NUM_RUNS;
        }
    } else {
        // Fallback timing for MPI stub (only rank 0)
        if (rank == 0) {
            int *original = malloc(array_size * sizeof(int));
            generateRandomArray(original, array_size, MAX_VALUE);
            
            for (int run = 0; run < NUM_RUNS; run++) {
                int *arr = malloc(array_size * sizeof(int));
                copyArray(original, arr, array_size);
                
                double start_time = getCurrentTime();
                parallelInsertionSortMPIAsc(arr, array_size);  // Will fallback
                double end_time = getCurrentTime();
                
                times[3] += (end_time - start_time);
                free(arr);
            }
            times[3] /= NUM_RUNS;
            free(original);
        }
    }
#else
    // Fallback timing for MPI stub (only rank 0)
    if (rank == 0) {
        int *original = malloc(array_size * sizeof(int));
        generateRandomArray(original, array_size, MAX_VALUE);
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            copyArray(original, arr, array_size);
            
            double start_time = getCurrentTime();
            parallelInsertionSortMPIAsc(arr, array_size);  // Will fallback
            double end_time = getCurrentTime();
            
            times[3] += (end_time - start_time);
            free(arr);
        }
        times[3] /= NUM_RUNS;
        free(original);
    }
#endif
    
    // Only rank 0 prints results
    if (rank == 0) {
        // Print results
        for (int i = 0; i < 4; i++) {
            double speedup = times[0] / times[i];  // Compare to sequential
            printf("%-15s | %-12.6f | %-10.2f\n", methods[i], times[i], speedup);
        }
        
        printf("\n" CYAN "=== PHÂN TÍCH ===" RESET "\n");
        printf("Hiệu suất tốt nhất: ");
        int best = 0;
        for (int i = 1; i < 4; i++) {
            if (times[i] < times[best]) best = i;
        }
        printf("%s (%.6f giây)\n", methods[best], times[best]);
        
        printf("Tăng tốc tốt nhất: %.2fx (%s)\n", times[0] / times[best], methods[best]);
        
#ifdef HAVE_MPI
        if (isMPIInitialized()) {
            printf("💡 MPI đang chạy với %d tiến trình\n", size);
        }
#endif
    }
}

// ========== HÀM GIAO DIỆN CHÍNH ==========

void overallTestOGT(void) {
    int choice;
    
    // Show library info at startup
    
    
#ifdef HAVE_MPI
    int rank, size;
    if (isMPIInitialized()) {
        getMPIInfo(&rank, &size);
        
        // Only rank 0 handles the interactive menu in MPI mode
        if (rank != 0) {
            // Non-root processes wait for commands from root
            while (1) {
                int mpi_choice;
                MPI_Bcast(&mpi_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
                
                if (mpi_choice == 0) break; // Exit signal
                
                switch (mpi_choice) {
                    case 4: // MPI functions
                        {
                            int sub_choice;
                            MPI_Bcast(&sub_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
                            if (sub_choice == 1) {
                                runMPIDemo();
                            } else if (sub_choice == 2) {
                                runMPIBenchmark();
                            }
                        }
                        break;
                    case 5: // Compare all
                        runAllComparison();
                        break;
                }
            }
            return; // Exit for non-root processes
        }
        
        // Root process continues with menu
        printf("\n" CYAN "Chạy trong chế độ MPI với %d tiến trình" RESET "\n", size);
    }
#endif
    
    do {
#ifdef HAVE_MPI
        int rank = 0, size = 1;
        if (isMPIInitialized()) {
            getMPIInfo(&rank, &size);
        }
        
        // Only rank 0 prints UI elements
        if (rank == 0) {
#endif
            printLibraryInfo();
            printf("\n" BLUE "=== THƯ VIỆN SORT OGT - MENU CHÍNH ===" RESET "\n");
            printf("1. 🔢 Tuần Tự (Sequential)\n");
            printf("2. 🚀 OpenMP\n");
            printf("3. 🧵 Pthreads\n");
            printf("4. 🌐 MPI\n");
            printf("5. 📊 So Sánh Tất Cả (Compare All)\n");
            printf("6. ℹ️  Thông Tin Hệ Thống (System Info)\n");
            printf("0. 🚪 Thoát (Exit)\n");
            printf("Lựa chọn: ");
            
            scanf("%d", &choice);
#ifdef HAVE_MPI
        } else {
            // Non-root processes don't print anything
            choice = 0; // Will be overwritten by broadcast
        }
#endif
        
#ifdef HAVE_MPI
        // Broadcast choice to all MPI processes
        if (isMPIInitialized()) {
            MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }
#endif
        
        switch (choice) {
            case 1: {
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
#endif
                        printf("\n" GREEN "=== TUẦN TỰ (SEQUENTIAL) ===" RESET "\n");
                        printf("1. Demo\n");
                        printf("2. Benchmark\n");
                        printf("Chọn (1-2): ");
#ifdef HAVE_MPI
                    }
                }
#endif
                int sub_choice;
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
                        scanf("%d", &sub_choice);
                    }
                    MPI_Bcast(&sub_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
                } else {
#endif
                    scanf("%d", &sub_choice);
#ifdef HAVE_MPI
                }
#endif
                
                if (sub_choice == 1) {
                    runSequentialDemo();
                } else if (sub_choice == 2) {
                    runSequentialBenchmark();
                } else {
#ifdef HAVE_MPI
                    if (isMPIInitialized()) {
                        int rank;
                        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                        if (rank == 0) {
#endif
                            printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
#ifdef HAVE_MPI
                        }
                    } else {
                        printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                    }
#endif
                }
                break;
            }
            
            case 2: {
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
#endif
                        printf("\n" CYAN "=== OPENMP ===" RESET "\n");
                        printf("1. Demo\n");
                        printf("2. Benchmark\n");
                        printf("Chọn (1-2): ");
#ifdef HAVE_MPI
                    }
                }
#endif
                int sub_choice;
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
                        scanf("%d", &sub_choice);
                    }
                    MPI_Bcast(&sub_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
                } else {
#endif
                    scanf("%d", &sub_choice);
#ifdef HAVE_MPI
                }
#endif
                
                if (sub_choice == 1) {
                    runOpenMPDemo();
                } else if (sub_choice == 2) {
                    runOpenMPBenchmark();
                } else {
#ifdef HAVE_MPI
                    if (isMPIInitialized()) {
                        int rank;
                        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                        if (rank == 0) {
#endif
                            printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
#ifdef HAVE_MPI
                        }
                    } else {
                        printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                    }
#endif
                }
                break;
            }
            
            case 3: {
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
#endif
                        printf("\n" BLUE "=== PTHREADS ===" RESET "\n");
                        printf("1. Demo\n");
                        printf("2. Benchmark\n");
                        printf("Chọn (1-2): ");
#ifdef HAVE_MPI
                    }
                }
#endif
                int sub_choice;
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
                        scanf("%d", &sub_choice);
                    }
                    MPI_Bcast(&sub_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
                } else {
#endif
                    scanf("%d", &sub_choice);
#ifdef HAVE_MPI
                }
#endif
                
                if (sub_choice == 1) {
                    runPthreadsDemo();
                } else if (sub_choice == 2) {
                    runPthreadsBenchmark();
                } else {
#ifdef HAVE_MPI
                    if (isMPIInitialized()) {
                        int rank;
                        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                        if (rank == 0) {
#endif
                            printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
#ifdef HAVE_MPI
                        }
                    } else {
                        printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                    }
#endif
                }
                break;
            }
            
                        case 4: {
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
#endif
                        printf("\n" YELLOW "=== MPI ===" RESET "\n");
                        printf("1. Demo\n");
                        printf("2. Benchmark\n");
                        printf("Chọn (1-2): ");
#ifdef HAVE_MPI
                    }
                }
#endif
                int sub_choice;
#ifdef HAVE_MPI
                if (isMPIInitialized()) {
                    int rank;
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    if (rank == 0) {
                        scanf("%d", &sub_choice);
                    }
                    MPI_Bcast(&sub_choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
                } else {
#endif
                    scanf("%d", &sub_choice);
#ifdef HAVE_MPI
                }
#endif
                
                if (sub_choice == 1) {
                    runMPIDemo();
                } else if (sub_choice == 2) {
                    runMPIBenchmark();
                } else {
#ifdef HAVE_MPI
                    if (isMPIInitialized()) {
                        int rank;
                        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                        if (rank == 0) {
#endif
                            printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
#ifdef HAVE_MPI
                        }
                    } else {
                        printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                    }
#endif
                }
                break;
            }
            
            case 5:
                runAllComparison();
                break;
                
            case 6:
                printSystemInformation();
                break;
                
            case 0:
                printf(GREEN "\n👋 Tạm biệt từ Thư viện Sort OGT!\n");
                printf("Cảm ơn bạn đã test!" RESET "\n");
                break;
                
            default:
                printf(RED "❌ Lựa chọn không hợp lệ! Vui lòng chọn 0-6." RESET "\n");
                break;
        }
        
        // Press enter to continue (except for exit)
        if (choice != 0) {
            printf("\n" YELLOW "Nhấn Enter để tiếp tục..." RESET);
            
            // Clear input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            // Wait for enter only in interactive mode
            if (isatty(0)) {  // Check if input is from terminal
                getchar();
            } else {
                printf("(Tự động tiếp tục trong chế độ không tương tác)\n");
            }
        }
        
    } while (choice != 0);
}

// ========== CÁC HÀM CŨ ĐỂ TƯƠNG THÍCH ==========
// Giữ những hàm này để tương thích ngược

void runBasicSortingDemo(void) {
    runSequentialDemo();
}

void runQuickParallelDemo(void) {
    runOpenMPDemo();
}

void runCompleteBenchmark(void) {
    runAllComparison();
}

void testSequentialUserInput(void) {
    runSequentialDemo();
}

void testParallelRandomArray(void) {
    runOpenMPDemo();
}

void testThreadPerformanceComparison(void) {
    runOpenMPBenchmark();
}

void runInteractiveTests(void) {
    overallTestOGT();
}

void runSingleBenchmark(int array_size) {
    printf("Benchmark đơn cho kích thước mảng %d:\n", array_size);
    runOpenMPBenchmark();
}

void demonstrateSortingCorrectness(void) {
    runAllComparison();
}

void demonstratePthreadsSort(void) {
    runPthreadsDemo();
}

void benchmarkPthreadsSort(int array_size, int num_threads) {
    printf("Benchmark Pthreads cho kích thước mảng %d với %d luồng:\n", array_size, num_threads);
    runPthreadsBenchmark();
}

void demonstrateMPISort(void) {
    runMPIDemo();
}

void benchmarkMPISort(int array_size) {
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("Benchmark MPI cho kích thước mảng %d với %d tiến trình:\n", array_size, size);
        printf("%-8s | %-12s | %-10s\n", "Kích Thước", "Thời Gian TB (s)", "Tăng Tốc");
        printf("-----------------------------------\n");
    }
    
    double total_time = 0.0;
    double sequential_time = 0.0;
    
    // First, get sequential baseline (only on rank 0)
    if (rank == 0) {
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
            generateRandomArray(arr, array_size, MAX_VALUE);
            
            double start_time = getCurrentTime();
            insertionSortAsc(arr, array_size);
            double end_time = getCurrentTime();
            
            sequential_time += (end_time - start_time);
            free(arr);
        }
        sequential_time /= NUM_RUNS;
    }
    
    // MPI benchmark
    for (int run = 0; run < NUM_RUNS; run++) {
        int *arr = NULL;
        
        // Only rank 0 generates and allocates the array
        if (rank == 0) {
            arr = malloc(array_size * sizeof(int));
            generateRandomArray(arr, array_size, MAX_VALUE);
        }
        
        double start_time = getCurrentTime();
        parallelInsertionSortMPIAsc(arr, array_size);
        double end_time = getCurrentTime();
        
        if (rank == 0) {
            total_time += (end_time - start_time);
            free(arr);
        }
    }
    
    if (rank == 0) {
        double avg_time = total_time / NUM_RUNS;
        double speedup = sequential_time / avg_time;
        printf("%-8d | %-12.6f | %-10.2f\n", array_size, avg_time, speedup);
    }
#else
    printf("Benchmark MPI không khả dụng - MPI chưa được biên dịch\n");
    printf("Kích thước mảng: %d - sẽ hiển thị so sánh MPI vs tuần tự\n", array_size);
#endif
}

void printMPISystemInfo(void) {
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("\n" CYAN "=== THÔNG TIN HỆ THỐNG MPI ===" RESET "\n");
        printf("Số tiến trình MPI: %d\n", size);
        
        // Get MPI version
        int version, subversion;
        MPI_Get_version(&version, &subversion);
        printf("Phiên bản MPI: %d.%d\n", version, subversion);
    }
#else
    printf(RED "Thông tin hệ thống MPI không khả dụng - MPI chưa được biên dịch\n" RESET);
#endif
} 