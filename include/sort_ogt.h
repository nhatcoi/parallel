#ifndef SORT_OGT_H
#define SORT_OGT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Conditional includes - Các include có điều kiện
#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef HAVE_MPI
#include <mpi.h>
#endif

// Thông tin thư viện
#define SORT_OGT_VERSION "1.0.0"
#define SORT_OGT_AUTHOR "OGT Team"

// Định nghĩa màu cho terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// ========== CÁC HÀM SẮP XẾP TUẦN TỰ ==========
void insertionSortAsc(int a[], int n);
void insertionSortDesc(int a[], int n);

// ========== CÁC HÀM SẮP XẾP SONG SONG ==========
// Triển khai OpenMP
void parallelInsertionSortAsc(int a[], int n, int num_threads);
void parallelInsertionSortDesc(int a[], int n, int num_threads);

// Triển khai Pthreads
void parallelInsertionSortPthreadsAsc(int a[], int n, int num_threads);
void parallelInsertionSortPthreadsDesc(int a[], int n, int num_threads);

// Triển khai MPI (luôn khả dụng, nhưng có stub khi MPI bị tắt)
void parallelInsertionSortMPIAsc(int a[], int n);
void parallelInsertionSortMPIDesc(int a[], int n);

// ========== CÁC HÀM TIỆN ÍCH ==========
double getCurrentTime(void);
void copyArray(int src[], int dest[], int n);
void generateRandomArray(int arr[], int n, int max_val);

// ========== CÁC HÀM BENCHMARK ==========
void printBenchmarkResults(const char* sort_type, int array_size, int threads, double avg_time, double speedup);
void runSingleBenchmark(int array_size);
void runDetailedBenchmarkAnalysis(void);
void demonstrateSortingCorrectness(void);
void printSystemInformation(void);
void runCompleteBenchmark(void);
void printLibraryInfo(void);
void runCustomThreadBenchmark(void);

// ========== CÁC HÀM DEMO ==========
void runBasicSortingDemo(void);
void runQuickParallelDemo(void);

// Các hàm demo Pthreads
void demonstratePthreadsSort(void);
void benchmarkPthreadsSort(int array_size, int num_threads);

// Các hàm demo MPI (luôn khả dụng, nhưng có stub khi MPI bị tắt)
void demonstrateMPISort(void);
void benchmarkMPISort(int array_size);
void runMPIBenchmark(void);
void printMPISystemInfo(void);
void runComprehensiveMPITest(void);

// Các hàm tiện ích MPI
int initializeMPI(int argc, char* argv[]);
void finalizeMPI(void);
void getMPIInfo(int* rank, int* size);
int isMPIInitialized(void);

// ========== CÁC HÀM TEST ==========
void testSequentialUserInput(void);
void testParallelRandomArray(void);
void testThreadPerformanceComparison(void);
void runInteractiveTests(void);

// ========== CÁC HÀM UI ==========
void overallTestOGT(void);

#endif // SORT_OGT_H 