#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sort_ogt.h"

int main(int argc, char* argv[]) {
    // Khởi tạo MPI nếu có và chạy với mpirun
#ifdef HAVE_MPI
    int mpi_init_result = initializeMPI(argc, argv);
    if (mpi_init_result != 0) {
        printf("Cảnh báo: Khởi tạo MPI thất bại, tiếp tục không dùng MPI\n");
    }
#endif

    // Chạy trình UI test chính
    overallTestOGT();

    // Đóng MPI nếu đã khởi tạo
#ifdef HAVE_MPI
    finalizeMPI();
#endif

    return 0;
}


// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>
// #include "sort_ogt.h"

// // Hàm kiểm tra mảng đã sắp xếp chưa
// int isSorted(int arr[], int n, int ascending) {
//     for (int i = 1; i < n; i++) {
//         if (ascending) {
//             if (arr[i] < arr[i-1]) return 0;
//         } else {
//             if (arr[i] > arr[i-1]) return 0;
//         }
//     }
//     return 1;
// }


// void printArray(int arr[], int n) {
//     printf(" - Mảng : ");
//     if (n <= 10) {
//         for (int i = 0; i < n; i++) {
//             printf("%d ", arr[i]);
//         }
//     } else {
//         // In 3 phần tử đầu
//         for (int i = 0; i < 3; i++) {
//             printf("%d ", arr[i]);
//         }
//         printf("... ");
//         // In 3 phần tử cuối
//         for (int i = n - 3; i < n; i++) {
//             printf("%d ", arr[i]);
//         }
//     }
//     printf("\n");
// }


// int main(int argc, char* argv[]) {
//     const int N = 1000;
//     const int NUM_THREADS = 4;
    
//     // Khởi tạo MPI nếu có
// #ifdef HAVE_MPI
//     initializeMPI(argc, argv);
// #endif

//     srand(time(NULL));
    
//     // Tạo mảng gốc
//     int original[N];
//     generateRandomArray(original, N, 10000);

//     printArray(original, N);
    
//     printf("=== Test 6 API sắp xếp với %d phần tử ===\n\n", N);
    
//     // Test 1: Sequential Ascending
//     int arr1[N];
//     copyArray(original, arr1, N);
//     clock_t start = clock();
//     insertionSortAsc(arr1, N);
//     clock_t end = clock();
//     double time1 = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("1. Sequential Asc:  %.4fs - %s\n", time1, isSorted(arr1, N, 1) ? "✓" : "✗");
//     printArray(arr1, N);
    
//     // Test 2: Sequential Descending
//     int arr2[N];
//     copyArray(original, arr2, N);
//     start = clock();
//     insertionSortDesc(arr2, N);
//     end = clock();
//     double time2 = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("2. Sequential Desc: %.4fs - %s\n", time2, isSorted(arr2, N, 0) ? "✓" : "✗");
//     printArray(arr2, N);
    
//     // Test 3: OpenMP Ascending
//     int arr3[N];
//     copyArray(original, arr3, N);
//     start = clock();
//     parallelInsertionSortAsc(arr3, N, NUM_THREADS);
//     end = clock();
//     double time3 = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("3. OpenMP Asc:     %.4fs - %s\n", time3, isSorted(arr3, N, 1) ? "✓" : "✗");
//     printArray(arr3, N);
//     // Test 4: OpenMP Descending
//     int arr4[N];
//     copyArray(original, arr4, N);
//     start = clock();
//     parallelInsertionSortDesc(arr4, N, NUM_THREADS);
//     end = clock();
//     double time4 = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("4. OpenMP Desc:    %.4fs - %s\n", time4, isSorted(arr4, N, 0) ? "✓" : "✗");
//     printArray(arr4, N);
    
//     // Test 5: Pthreads Ascending
//     int arr5[N];
//     copyArray(original, arr5, N);
//     start = clock();
//     parallelInsertionSortPthreadsAsc(arr5, N, NUM_THREADS);
//     end = clock();
//     double time5 = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("5. Pthreads Asc:   %.4fs - %s\n", time5, isSorted(arr5, N, 1) ? "✓" : "✗");
//     printArray(arr5, N);    
    
//     // Test 6: Pthreads Descending
//     int arr6[N];
//     copyArray(original, arr6, N);
//     start = clock();
//     parallelInsertionSortPthreadsDesc(arr6, N, NUM_THREADS);
//     end = clock();
//     double time6 = ((double)(end - start)) / CLOCKS_PER_SEC;
//     printf("6. Pthreads Desc:  %.4fs - %s\n", time6, isSorted(arr6, N, 0) ? "✓" : "✗");
//     printArray(arr6, N);
//     // Test MPI (chỉ khi có MPI)
// #ifdef HAVE_MPI
//     int rank;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     if (rank == 0) {
//         // Test 7: MPI Ascending
//         int arr7[N];
//         copyArray(original, arr7, N);
//         start = clock();
//         parallelInsertionSortMPIAsc(arr7, N);
//         end = clock();
//         double time7 = ((double)(end - start)) / CLOCKS_PER_SEC;
//         printf("7. MPI Asc:        %.4fs - %s\n", time7, isSorted(arr7, N, 1) ? "✓" : "✗");
//         printArray(arr7, N);
//         // Test 8: MPI Descending
//         int arr8[N];
//         copyArray(original, arr8, N);
//         start = clock();
//         parallelInsertionSortMPIDesc(arr8, N);
//         end = clock();
//         double time8 = ((double)(end - start)) / CLOCKS_PER_SEC;
//         printf("8. MPI Desc:       %.4fs - %s\n", time8, isSorted(arr8, N, 0) ? "✓" : "✗");
//         printArray(arr8, N);
//     }
// #endif
    
//     printf("\n=== Test hoàn thành ===\n");
    
// #ifdef HAVE_MPI
//     finalizeMPI();
// #endif

//     return 0;
// }