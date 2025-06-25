#include "../../include/sort_ogt.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Test tuần tự với mảng do người dùng nhập
void testSequentialUserInput(void) {
    int n;
    printf("\n=== TEST TUẦN TỰ - NHẬP MẢNG ===\n");
    printf("Nhập số phần tử mảng: ");
    scanf("%d", &n);
    
    if (n <= 0 || n > 1000) {
        printf("❌ Số phần tử không hợp lệ (1-1000)!\n");
        return;
    }
    
    int *arr = malloc(n * sizeof(int));
    int *backup = malloc(n * sizeof(int));
    
    printf("Nhập %d phần tử của mảng:\n", n);
    for (int i = 0; i < n; i++) {
        printf("arr[%d] = ", i);
        scanf("%d", &arr[i]);
    }
    
    // Backup original array
    copyArray(arr, backup, n);
    
    // Hiển thị mảng gốc
    printf("\n📋 Mảng gốc: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    // Test sắp xếp tăng dần
    double start_time = getCurrentTime();
    insertionSortAsc(arr, n);
    double end_time = getCurrentTime();
    
    printf("✅ Sắp xếp tăng dần: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n⏱️  Thời gian: %.6f giây\n", end_time - start_time);
    
    // Restore và test sắp xếp giảm dần
    copyArray(backup, arr, n);
    
    start_time = getCurrentTime();
    insertionSortDesc(arr, n);
    end_time = getCurrentTime();
    
    printf("✅ Sắp xếp giảm dần: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n⏱️  Thời gian: %.6f giây\n", end_time - start_time);
    
    free(arr);
    free(backup);
}

// Test song song với mảng ngẫu nhiên
void testParallelRandomArray(void) {
    int n, max_val, num_threads;
    
    printf("\n=== TEST SONG SONG - MẢNG NGẪU NHIÊN ===\n");
    printf("Nhập số phần tử mảng (1000-100000): ");
    scanf("%d", &n);
    
    if (n < 1000 || n > 100000) {
        printf("❌ Số phần tử không hợp lệ (1000-100000)!\n");
        return;
    }
    
    printf("Nhập giá trị max cho phần tử (1-10000): ");
    scanf("%d", &max_val);
    
    if (max_val < 1 || max_val > 10000) {
        printf("❌ Giá trị max không hợp lệ (1-10000)!\n");
        return;
    }
    
    printf("Nhập số threads (1-12): ");
    scanf("%d", &num_threads);
    
    if (num_threads < 1 || num_threads > 12) {
        printf("❌ Số threads không hợp lệ (1-12)!\n");
        return;
    }
    
    // Khởi tạo random seed
    srand(time(NULL));
    
    // Tạo mảng và backup
    int *arr_seq = malloc(n * sizeof(int));
    int *arr_par = malloc(n * sizeof(int));
    
    generateRandomArray(arr_seq, n, max_val);
    copyArray(arr_seq, arr_par, n);
    
    printf("\n📊 Đã tạo mảng %d phần tử với giá trị 0-%d\n", n, max_val - 1);
    
    // Hiển thị một phần mảng nếu nhỏ
    if (n <= 20) {
        printf("📋 Mảng gốc: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr_seq[i]);
        }
        printf("\n");
    }
    
    // Test sequential
    printf("\n🔄 Testing Sequential Sort...\n");
    double seq_start = getCurrentTime();
    insertionSortAsc(arr_seq, n);
    double seq_end = getCurrentTime();
    double seq_time = seq_end - seq_start;
    
    printf("✅ Sequential time: %.6f giây\n", seq_time);
    
    // Test parallel
    printf("🚀 Testing Parallel Sort (%d threads)...\n", num_threads);
    double par_start = getCurrentTime();
    parallelInsertionSortAsc(arr_par, n, num_threads);
    double par_end = getCurrentTime();
    double par_time = par_end - par_start;
    
    printf("✅ Parallel time: %.6f giây\n", par_time);
    
    // Verify correctness
    int correct = 1;
    for (int i = 0; i < n; i++) {
        if (arr_seq[i] != arr_par[i]) {
            correct = 0;
            break;
        }
    }
    
    printf("\n📊 KẾT QUẢ:\n");
    printf("Sequential: %.6f giây\n", seq_time);
    printf("Parallel:   %.6f giây (%d threads)\n", par_time, num_threads);
    printf("Speedup:    %.2fx\n", seq_time / par_time);
    printf("Kết quả:    %s\n", correct ? "✅ Chính xác" : "❌ Sai lệch");
    
    // Hiển thị kết quả nếu mảng nhỏ
    if (n <= 20) {
        printf("\n📋 Kết quả sắp xếp: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr_par[i]);
        }
        printf("\n");
    }
    
    free(arr_seq);
    free(arr_par);
}

// Test so sánh performance với nhiều thread counts
void testThreadPerformanceComparison(void) {
    int n = 50000; // Fixed size for comparison
    int max_val = 1000;
    int thread_counts[] = {1, 3, 5, 7, 9, 11};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("\n=== SO SÁNH HIỆU NĂNG THREADS ===\n");
    printf("Mảng size: %d phần tử\n", n);
    printf("Giá trị max: %d\n", max_val);
    printf("Runs: 3 lần cho mỗi thread count\n\n");
    
    srand(time(NULL));
    
    // Create original array
    int *original = malloc(n * sizeof(int));
    generateRandomArray(original, n, max_val);
    
    printf("Thread Count | Avg Time (s) | Speedup\n");
    printf("-------------|--------------|--------\n");
    
    double baseline_time = 0;
    
    for (int t = 0; t < num_tests; t++) {
        int threads = thread_counts[t];
        double total_time = 0;
        
        // Run 3 times and average
        for (int run = 0; run < 3; run++) {
            int *test_array = malloc(n * sizeof(int));
            copyArray(original, test_array, n);
            
            double start_time = getCurrentTime();
            if (threads == 1) {
                insertionSortAsc(test_array, n);
            } else {
                parallelInsertionSortAsc(test_array, n, threads);
            }
            double end_time = getCurrentTime();
            
            total_time += (end_time - start_time);
            free(test_array);
        }
        
        double avg_time = total_time / 3.0;
        
        if (threads == 1) {
            baseline_time = avg_time;
            printf("%-12d | %-12.6f | %-7s\n", threads, avg_time, "1.00x");
        } else {
            double speedup = baseline_time / avg_time;
            printf("%-12d | %-12.6f | %-7.2fx\n", threads, avg_time, speedup);
        }
    }
    
    free(original);
}

// Menu chính cho test functions
void runInteractiveTests(void) {
    int choice;
    
    do {
        printf("\n" BLUE "=== SORT OGT TESTING MENU ===" RESET "\n");
        printf("1. 📝 Test Tuần Tự (Nhập Mảng)\n");
        printf("2. 🎲 Test Song Song (Mảng Ngẫu Nhiên)\n");
        printf("3. 📊 So Sánh Performance Threads\n");
        printf("0. 🚪 Quay Lại Menu Chính\n");
        printf("Lựa chọn: ");
        
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                testSequentialUserInput();
                break;
            case 2:
                testParallelRandomArray();
                break;
            case 3:
                testThreadPerformanceComparison();
                break;
            case 0:
                printf("Quay lại menu chính...\n");
                break;
            default:
                printf("❌ Lựa chọn không hợp lệ!\n");
                break;
        }
    } while (choice != 0);
} 