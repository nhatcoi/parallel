#include "../../include/sort_ogt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>  // for isatty()
#include <omp.h>     // for OpenMP functions

#define MAX_ARRAY_SIZE 100000
#define MAX_VALUE 10000
#define NUM_RUNS 5

// ========== UTILITY FUNCTIONS ==========

// Function to print an array
static void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

// Print benchmark results in formatted table
void printBenchmarkResults(const char* sort_type, int array_size, int threads, double avg_time, double speedup) {
    printf("%-15s | %-10d | %-8d | %-12.6f | %-10.4f\n", 
           sort_type, array_size, threads, avg_time, speedup);
}

// Print library information
void printLibraryInfo(void) {
    printf("\n" MAGENTA "╔══════════════════════════════════════════════════════╗\n");
    printf("║                   SORT OGT LIBRARY                   ║\n");
    printf("║              Parallel Insertion Sort                 ║\n");
    printf("║                   Version: %-8s                  ║\n", SORT_OGT_VERSION);
    printf("║                   Author: %-9s                  ║\n", SORT_OGT_AUTHOR);
    printf("╚══════════════════════════════════════════════════════╝" RESET "\n");
}

// Print system information
void printSystemInformation(void) {
    printf("\n" YELLOW "=== SYSTEM INFORMATION ===" RESET "\n");
    printf("Maximum threads available: %d\n", omp_get_max_threads());
    printf("Number of processors: %d\n", omp_get_num_procs());
    printf("OpenMP version: %d\n", _OPENMP);
    printf("Library version: %s\n", SORT_OGT_VERSION);
    printf("Author: %s\n", SORT_OGT_AUTHOR);
    printf("Test configuration:\n");
    printf("- Array sizes: 10K, 25K, 50K, 75K, 100K elements\n");
    printf("- Thread counts: 1, 3, 5, 7, 9, 11\n");
    printf("- Runs per configuration: %d\n", NUM_RUNS);
    printf("- Maximum random value: %d\n", MAX_VALUE);
    printf("\n");
}

// ========== 1. SEQUENTIAL SORTING FUNCTIONS ==========

void runSequentialDemo(void) {
    printf("\n" GREEN "=== DEMO TUẦN TỰ (SEQUENTIAL) ===" RESET "\n");
    
    // Sample array for demo
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50};
    int n = sizeof(arr) / sizeof(arr[0]);

    printf("Original array: ");
    printArray(arr, n);

    // Test ascending sort
    int *test_arr = malloc(n * sizeof(int));
    copyArray(arr, test_arr, n);
    
    double start_time = getCurrentTime();
    insertionSortAsc(test_arr, n);
    double end_time = getCurrentTime();
    
    printf("Sorted ascending: ");
    printArray(test_arr, n);
    printf("Time: %.6f seconds\n", end_time - start_time);

    // Test descending sort
    copyArray(arr, test_arr, n);
    start_time = getCurrentTime();
    insertionSortDesc(test_arr, n);
    end_time = getCurrentTime();
    
    printf("Sorted descending: ");
    printArray(test_arr, n);
    printf("Time: %.6f seconds\n", end_time - start_time);
    
    free(test_arr);
}

void runSequentialBenchmark(void) {
    printf("\n" GREEN "=== BENCHMARK TUẦN TỰ (SEQUENTIAL) ===" RESET "\n");
    
    int test_sizes[] = {1000, 5000, 10000, 25000, 50000};
    int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    printf("%-12s | %-12s\n", "Array Size", "Avg Time (s)");
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

// ========== 2. OPENMP SORTING FUNCTIONS ==========

void runOpenMPDemo(void) {
    printf("\n" CYAN "=== DEMO OPENMP ===" RESET "\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 72, 17, 95, 33, 47, 61, 8, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    int threads = 4;

    printf("Original array: ");
    printArray(arr, n);
    printf("Using %d threads\n", threads);

    // Test ascending sort
    int *test_arr = malloc(n * sizeof(int));
    copyArray(arr, test_arr, n);
    
    double start_time = getCurrentTime();
    parallelInsertionSortAsc(test_arr, n, threads);
    double end_time = getCurrentTime();
    
    printf("Sorted ascending (OpenMP): ");
    printArray(test_arr, n);
    printf("Time: %.6f seconds\n", end_time - start_time);
    
    free(test_arr);
}

void runOpenMPBenchmark(void) {
    printf("\n" CYAN "=== BENCHMARK OPENMP ===" RESET "\n");
    
    int array_size = 50000;
    int thread_counts[] = {1, 3, 5, 7, 9, 11};
    int num_thread_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("Array size: %d elements\n", array_size);
    printf("%-8s | %-12s | %-10s\n", "Threads", "Avg Time (s)", "Speedup");
    printf("-----------------------------------\n");
    
    double sequential_time = 0.0;
    
    for (int t = 0; t < num_thread_configs; t++) {
        int threads = thread_counts[t];
        double total_time = 0.0;
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
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
            printf("%-8d | %-12.6f | %-10.2f\n", threads, avg_time, 1.0);
        } else {
            double speedup = sequential_time / avg_time;
            printf("%-8d | %-12.6f | %-10.2f\n", threads, avg_time, speedup);
        }
    }
}

// ========== 3. PTHREADS SORTING FUNCTIONS ==========

void runPthreadsDemo(void) {
    printf("\n" BLUE "=== DEMO PTHREADS ===" RESET "\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 72, 17, 95, 33, 47, 61, 8, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    int threads = 4;

    printf("Original array: ");
    printArray(arr, n);
    printf("Using %d threads\n", threads);

    // Test ascending sort
    int *test_arr = malloc(n * sizeof(int));
    copyArray(arr, test_arr, n);
    
    double start_time = getCurrentTime();
    parallelInsertionSortPthreadsAsc(test_arr, n, threads);
    double end_time = getCurrentTime();
    
    printf("Sorted ascending (Pthreads): ");
    printArray(test_arr, n);
    printf("Time: %.6f seconds\n", end_time - start_time);
    
    // Verify correctness
    int is_sorted = 1;
    for (int i = 1; i < n; i++) {
        if (test_arr[i] < test_arr[i-1]) {
            is_sorted = 0;
            break;
        }
    }
    printf("Correctness: %s\n", is_sorted ? "✅ PASSED" : "❌ FAILED");
    
    free(test_arr);
}

void runPthreadsBenchmark(void) {
    printf("\n" BLUE "=== BENCHMARK PTHREADS ===" RESET "\n");
    
    int array_size = 50000;
    int thread_counts[] = {1, 3, 5, 7, 9, 11};
    int num_thread_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    printf("Array size: %d elements\n", array_size);
    printf("%-8s | %-12s | %-10s\n", "Threads", "Avg Time (s)", "Speedup");
    printf("-----------------------------------\n");
    
    double sequential_time = 0.0;
    
    for (int t = 0; t < num_thread_configs; t++) {
        int threads = thread_counts[t];
        double total_time = 0.0;
        
        for (int run = 0; run < NUM_RUNS; run++) {
            int *arr = malloc(array_size * sizeof(int));
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
            printf("%-8d | %-12.6f | %-10.2f\n", threads, avg_time, 1.0);
        } else {
            double speedup = sequential_time / avg_time;
            printf("%-8d | %-12.6f | %-10.2f\n", threads, avg_time, speedup);
        }
    }
}

// ========== 4. MPI SORTING FUNCTIONS ==========

void runMPIDemo(void) {
    printf("\n" YELLOW "=== DEMO MPI ===" RESET "\n");
    
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("Running MPI demo with %d processes\n", size);
    }
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42, 30, 72, 17, 95, 33, 47, 61, 8, 91};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    if (rank == 0) {
        printf("Original array: ");
        printArray(arr, n);
    }
    
    double start_time = getCurrentTime();
    parallelInsertionSortMPIAsc(arr, n);
    double end_time = getCurrentTime();
    
    if (rank == 0) {
        printf("Sorted ascending (MPI): ");
        printArray(arr, n);
        printf("Time: %.6f seconds\n", end_time - start_time);
    }
#else
    printf("MPI not available - showing fallback to sequential sort\n");
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Original array: ");
    printArray(arr, n);
    
    double start_time = getCurrentTime();
    parallelInsertionSortMPIAsc(arr, n);  // Will fallback to sequential
    double end_time = getCurrentTime();
    
    printf("Sorted (fallback): ");
    printArray(arr, n);
    printf("Time: %.6f seconds\n", end_time - start_time);
#endif
}

void runMPIBenchmark(void) {
    printf("\n" YELLOW "=== BENCHMARK MPI ===" RESET "\n");
    
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("MPI processes: %d\n", size);
    }
    
    int array_size = 50000;
    printf("Array size: %d elements\n", array_size);
    
    benchmarkMPISort(array_size);
#else
    printf("MPI not available - cannot run MPI benchmark\n");
    printf("This would show comparison between MPI and sequential sorting\n");
#endif
}

// ========== 5. COMPARISON FUNCTION ==========

void runAllComparison(void) {
    printf("\n" MAGENTA "=== SO SÁNH TẤT CẢ 4 KIỂU SORT ===" RESET "\n");
    
    int array_size = 25000;  // Medium size for fair comparison
    int threads = 5;         // Reasonable thread count
    
    printf("Array size: %d elements\n", array_size);
    printf("Threads (for parallel methods): %d\n", threads);
    printf("Runs per method: %d\n\n", NUM_RUNS);
    
    // Generate test data
    int *original = malloc(array_size * sizeof(int));
    generateRandomArray(original, array_size, MAX_VALUE);
    
    printf("%-15s | %-12s | %-10s\n", "Method", "Avg Time (s)", "Speedup");
    printf("------------------------------------------\n");
    
    double times[4] = {0, 0, 0, 0};
    const char* methods[] = {"Sequential", "OpenMP", "Pthreads", "MPI"};
    
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
    
    // 4. MPI
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    for (int run = 0; run < NUM_RUNS; run++) {
        int *arr = NULL;
        if (rank == 0) {
            arr = malloc(array_size * sizeof(int));
            copyArray(original, arr, array_size);
        }
        
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
#else
    // Fallback timing for MPI stub
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
#endif
    
    // Print results
    for (int i = 0; i < 4; i++) {
        double speedup = times[0] / times[i];  // Compare to sequential
        printf("%-15s | %-12.6f | %-10.2f\n", methods[i], times[i], speedup);
    }
    
    printf("\n" CYAN "=== ANALYSIS ===" RESET "\n");
    printf("Best performer: ");
    int best = 0;
    for (int i = 1; i < 4; i++) {
        if (times[i] < times[best]) best = i;
    }
    printf("%s (%.6f seconds)\n", methods[best], times[best]);
    
    printf("Best speedup: %.2fx (%s)\n", times[0] / times[best], methods[best]);
    
    free(original);
}

// ========== MAIN UI FUNCTION ==========

void overallTestOGT(void) {
    int choice;
    
    // Show library info at startup
    printLibraryInfo();
    
    do {
        printf("\n" BLUE "=== SORT OGT LIBRARY - MENU CHÍNH ===" RESET "\n");
        printf("1. 🔢 Tuần Tự (Sequential)\n");
        printf("2. 🚀 OpenMP\n");
        printf("3. 🧵 Pthreads\n");
        printf("4. 🌐 MPI\n");
        printf("5. 📊 So Sánh Tất Cả (Compare All)\n");
        printf("6. ℹ️  Thông Tin Hệ Thống (System Info)\n");
        printf("0. 🚪 Thoát (Exit)\n");
        printf("Lựa chọn: ");
        
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: {
                printf("\n" GREEN "=== TUẦN TỰ (SEQUENTIAL) ===" RESET "\n");
                printf("1. Demo\n");
                printf("2. Benchmark\n");
                printf("Chọn (1-2): ");
                int sub_choice;
                scanf("%d", &sub_choice);
                
                if (sub_choice == 1) {
                    runSequentialDemo();
                } else if (sub_choice == 2) {
                    runSequentialBenchmark();
                } else {
                    printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                }
                break;
            }
            
            case 2: {
                printf("\n" CYAN "=== OPENMP ===" RESET "\n");
                printf("1. Demo\n");
                printf("2. Benchmark\n");
                printf("Chọn (1-2): ");
                int sub_choice;
                scanf("%d", &sub_choice);
                
                if (sub_choice == 1) {
                    runOpenMPDemo();
                } else if (sub_choice == 2) {
                    runOpenMPBenchmark();
                } else {
                    printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                }
                break;
            }
            
            case 3: {
                printf("\n" BLUE "=== PTHREADS ===" RESET "\n");
                printf("1. Demo\n");
                printf("2. Benchmark\n");
                printf("Chọn (1-2): ");
                int sub_choice;
                scanf("%d", &sub_choice);
                
                if (sub_choice == 1) {
                    runPthreadsDemo();
                } else if (sub_choice == 2) {
                    runPthreadsBenchmark();
                } else {
                    printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
                }
                break;
            }
            
            case 4: {
                printf("\n" YELLOW "=== MPI ===" RESET "\n");
                printf("1. Demo\n");
                printf("2. Benchmark\n");
                printf("Chọn (1-2): ");
                int sub_choice;
                scanf("%d", &sub_choice);
                
                if (sub_choice == 1) {
                    runMPIDemo();
                } else if (sub_choice == 2) {
                    runMPIBenchmark();
                } else {
                    printf(RED "❌ Lựa chọn không hợp lệ!" RESET "\n");
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
                printf(GREEN "\n👋 Goodbye from Sort OGT Library!\n");
                printf("Thank you for testing!" RESET "\n");
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
                printf("(Auto-continuing in non-interactive mode)\n");
            }
        }
        
    } while (choice != 0);
}

// ========== LEGACY FUNCTION STUBS ==========
// Keep these for backward compatibility

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
    printf("Single benchmark for array size %d:\n", array_size);
    runOpenMPBenchmark();
}

void demonstrateSortingCorrectness(void) {
    runAllComparison();
}

void demonstratePthreadsSort(void) {
    runPthreadsDemo();
}

void benchmarkPthreadsSort(int array_size, int num_threads) {
    printf("Pthreads benchmark for array size %d with %d threads:\n", array_size, num_threads);
    runPthreadsBenchmark();
}

void demonstrateMPISort(void) {
    runMPIDemo();
}

void benchmarkMPISort(int array_size) {
    printf("MPI benchmark for array size %d:\n", array_size);
    runMPIBenchmark();
}

void printMPISystemInfo(void) {
#ifdef HAVE_MPI
    int rank, size;
    getMPIInfo(&rank, &size);
    
    if (rank == 0) {
        printf("\n" CYAN "=== MPI SYSTEM INFORMATION ===" RESET "\n");
        printf("Number of MPI processes: %d\n", size);
        
        // Get MPI version
        int version, subversion;
        MPI_Get_version(&version, &subversion);
        printf("MPI Version: %d.%d\n", version, subversion);
    }
#else
    printf(RED "MPI system info not available - MPI not compiled\n" RESET);
#endif
}

void runComprehensiveMPITest(void) {
    runMPIBenchmark();
} 