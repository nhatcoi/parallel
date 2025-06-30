#ifndef SORT_OGT_H
#define SORT_OGT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Conditional includes
#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef HAVE_MPI
#include <mpi.h>
#endif

// Library information
#define SORT_OGT_VERSION "1.0.0"
#define SORT_OGT_AUTHOR "OGT Team"

// Color definitions for terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// ========== SEQUENTIAL SORTING FUNCTIONS ==========
void insertionSortAsc(int a[], int n);
void insertionSortDesc(int a[], int n);

// ========== PARALLEL SORTING FUNCTIONS ==========
// OpenMP implementations
void parallelInsertionSortAsc(int a[], int n, int num_threads);
void parallelInsertionSortDesc(int a[], int n, int num_threads);

// Pthreads implementations
void parallelInsertionSortPthreadsAsc(int a[], int n, int num_threads);
void parallelInsertionSortPthreadsDesc(int a[], int n, int num_threads);

// MPI implementations (available always, but with stubs when MPI disabled)
void parallelInsertionSortMPIAsc(int a[], int n);
void parallelInsertionSortMPIDesc(int a[], int n);

// ========== UTILITY FUNCTIONS ==========
double getCurrentTime(void);
void copyArray(int src[], int dest[], int n);
void generateRandomArray(int arr[], int n, int max_val);

// ========== BENCHMARK FUNCTIONS ==========
void printBenchmarkResults(const char* sort_type, int array_size, int threads, double avg_time, double speedup);
void runSingleBenchmark(int array_size);
void runDetailedBenchmarkAnalysis(void);
void demonstrateSortingCorrectness(void);
void printSystemInformation(void);
void runCompleteBenchmark(void);
void printLibraryInfo(void);
void runCustomThreadBenchmark(void);

// ========== DEMO FUNCTIONS ==========
void runBasicSortingDemo(void);
void runQuickParallelDemo(void);

// Pthreads demo functions
void demonstratePthreadsSort(void);
void benchmarkPthreadsSort(int array_size, int num_threads);

// MPI demo functions (available always, but with stubs when MPI disabled)
void demonstrateMPISort(void);
void benchmarkMPISort(int array_size);
void runMPIBenchmark(void);
void printMPISystemInfo(void);
void runComprehensiveMPITest(void);

// MPI utility functions
int initializeMPI(int argc, char* argv[]);
void finalizeMPI(void);
void getMPIInfo(int* rank, int* size);
int isMPIInitialized(void);

// ========== TEST FUNCTIONS ==========
void testSequentialUserInput(void);
void testParallelRandomArray(void);
void testThreadPerformanceComparison(void);
void runInteractiveTests(void);

// ========== UI FUNCTIONS ==========
void overallTestOGT(void);

#endif // SORT_OGT_H 