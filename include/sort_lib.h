#ifndef SORT_LIB_H
#define SORT_LIB_H

// Function to sort an array in ascending order using insertion sort
void insertionSortAsc(int a[], int n);

// Function to sort an array in descending order using insertion sort
void insertionSortDesc(int a[], int n);

// Parallel insertion sort functions with OpenMP
void parallelInsertionSortAsc(int a[], int n, int num_threads);
void parallelInsertionSortDesc(int a[], int n, int num_threads);

// Utility functions for benchmarking
double getCurrentTime(void);
void copyArray(int src[], int dest[], int n);
void generateRandomArray(int arr[], int n, int max_val);

#endif // SORT_LIB_H 