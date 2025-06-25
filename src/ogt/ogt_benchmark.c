#include "../../include/sort_ogt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define MAX_ARRAY_SIZE 100000
#define MAX_VALUE 10000
#define NUM_RUNS 5

// Print benchmark results in formatted table
void printBenchmarkResults(const char* sort_type, int array_size, int threads, double avg_time, double speedup) {
    printf("%-15s | %-10d | %-8d | %-12.6f | %-10.4f\n", 
           sort_type, array_size, threads, avg_time, speedup);
}

// Run benchmark for a single array size
void runSingleBenchmark(int array_size) {
    printf("\n=== BENCHMARK RESULTS FOR ARRAY SIZE: %d ===\n", array_size);
    printf("%-15s | %-10s | %-8s | %-12s | %-10s\n", 
           "Sort Type", "Array Size", "Threads", "Avg Time (s)", "Speedup");
    printf("------------------------------------------------------------------------\n");
    
    int *original_array = malloc(array_size * sizeof(int));
    int *test_array = malloc(array_size * sizeof(int));
    int thread_counts[] = {1, 3, 5, 7, 9, 11};
    int num_thread_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    
    // Generate random array
    srand(time(NULL));
    generateRandomArray(original_array, array_size, MAX_VALUE);
    
    double sequential_time = 0.0;
    
    // Test each thread configuration
    for (int t = 0; t < num_thread_configs; t++) {
        int threads = thread_counts[t];
        double total_time = 0.0;
        
        // Run multiple times and take average
        for (int run = 0; run < NUM_RUNS; run++) {
            copyArray(original_array, test_array, array_size);
            
            double start_time = getCurrentTime();
            
            if (threads == 1) {
                // Sequential version
                insertionSortAsc(test_array, array_size);
            } else {
                // Parallel version
                parallelInsertionSortAsc(test_array, array_size, threads);
            }
            
            double end_time = getCurrentTime();
            total_time += (end_time - start_time);
        }
        
        double avg_time = total_time / NUM_RUNS;
        
        if (threads == 1) {
            sequential_time = avg_time;
            printBenchmarkResults("Sequential", array_size, threads, avg_time, 1.0);
        } else {
            double speedup = sequential_time / avg_time;
            printBenchmarkResults("Parallel", array_size, threads, avg_time, speedup);
        }
    }
    
    free(original_array);
    free(test_array);
}

// Run detailed analysis with multiple array sizes
void runDetailedBenchmarkAnalysis(void) {
    printf("\n" CYAN "=== DETAILED PERFORMANCE ANALYSIS ===" RESET "\n");
    printf("Testing with different array sizes and thread counts...\n");
    
    int test_sizes[] = {10000, 25000, 50000, 75000, 100000};
    int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        runSingleBenchmark(test_sizes[i]);
    }
}

// Demonstrate sorting correctness
void demonstrateSortingCorrectness(void) {
    printf("\n" GREEN "=== CORRECTNESS VERIFICATION ===" RESET "\n");
    
    int test_array[] = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42};
    int n = sizeof(test_array) / sizeof(test_array[0]);
    int *parallel_test = malloc(n * sizeof(int));
    int *sequential_test = malloc(n * sizeof(int));
    
    // Copy original array
    copyArray(test_array, parallel_test, n);
    copyArray(test_array, sequential_test, n);
    
    printf("Original array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", test_array[i]);
    }
    printf("\n");
    
    // Test sequential sort
    insertionSortAsc(sequential_test, n);
    printf("Sequential sort: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", sequential_test[i]);
    }
    printf("\n");
    
    // Test parallel sort with 3 threads
    parallelInsertionSortAsc(parallel_test, n, 3);
    printf("Parallel sort (3 threads): ");
    for (int i = 0; i < n; i++) {
        printf("%d ", parallel_test[i]);
    }
    printf("\n");
    
    // Verify correctness
    int correct = 1;
    for (int i = 0; i < n; i++) {
        if (sequential_test[i] != parallel_test[i]) {
            correct = 0;
            break;
        }
    }
    
    printf("Correctness check: %s\n", correct ? "PASSED ✓" : "FAILED ✗");
    
    free(parallel_test);
    free(sequential_test);
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

// Complete benchmark suite (equivalent to old benchmark.c main())
void runCompleteBenchmark(void) {
    printf(MAGENTA "PARALLEL INSERTION SORT PERFORMANCE BENCHMARK\n");
    printf("==============================================\n" RESET);
    
    printSystemInformation();
    demonstrateSortingCorrectness();
    runDetailedBenchmarkAnalysis();
    
    printf("\n" BLUE "=== SUMMARY ===" RESET "\n");
    printf("1. Sequential version provides baseline performance\n");
    printf("2. Parallel versions show speedup with optimal thread counts\n");
    printf("3. Performance varies with array size and thread count\n");
    printf("4. Best performance typically achieved with thread count close to CPU cores\n");
    printf("5. Overhead may reduce efficiency for small arrays or too many threads\n");
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