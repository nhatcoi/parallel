#include "sort_ogt.h"
#include <string.h>

#ifdef HAVE_MPI
#include <mpi.h>
#include <sys/time.h>

// Structure for chunk information in MPI
typedef struct {
    int start;
    int end;
    int size;
    int rank;
} MPIChunkInfo;

/**
 * Merge two sorted arrays (MPI version)
 */
void merge_two_arrays_mpi(int arr[], int left, int mid, int right, int ascending) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Create temporary arrays
    int* left_arr = (int*)malloc(n1 * sizeof(int));
    int* right_arr = (int*)malloc(n2 * sizeof(int));
    
    // Copy data to temporary arrays
    for (i = 0; i < n1; i++)
        left_arr[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        right_arr[j] = arr[mid + 1 + j];
    
    // Merge the temporary arrays back into arr[left..right]
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
    
    // Copy remaining elements
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
 * Merge sorted chunks collected from all MPI processes
 */
void merge_mpi_chunks(int arr[], int* chunk_sizes, int num_procs, int total_size, int ascending) {
    if (num_procs <= 1) return;
    
    // Calculate chunk positions
    int* chunk_starts = (int*)malloc(num_procs * sizeof(int));
    chunk_starts[0] = 0;
    for (int i = 1; i < num_procs; i++) {
        chunk_starts[i] = chunk_starts[i-1] + chunk_sizes[i-1];
    }
    
    // Iteratively merge chunks using binary merge approach
    int active_chunks = num_procs;
    while (active_chunks > 1) {
        int new_active = (active_chunks + 1) / 2;
        
        for (int i = 0; i < new_active; i++) {
            int left_idx = i * 2;
            int right_idx = left_idx + 1;
            
            if (right_idx < active_chunks) {
                // Merge chunk left_idx and right_idx
                int left = chunk_starts[left_idx];
                int mid = chunk_starts[left_idx] + chunk_sizes[left_idx] - 1;
                int right = chunk_starts[right_idx] + chunk_sizes[right_idx] - 1;
                
                merge_two_arrays_mpi(arr, left, mid, right, ascending);
                
                // Update chunk info for merged chunk
                chunk_sizes[i] = chunk_sizes[left_idx] + chunk_sizes[right_idx];
                chunk_starts[i] = chunk_starts[left_idx];
            } else {
                // Odd number of chunks, carry forward the last one
                chunk_sizes[i] = chunk_sizes[left_idx];
                chunk_starts[i] = chunk_starts[left_idx];
            }
        }
        
        active_chunks = new_active;
    }
    
    free(chunk_starts);
}

/**
 * Core MPI implementation function
 */
void parallelInsertionSortMPI(int a[], int n, int ascending) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (n <= 1) return;
    
    // Calculate chunk sizes with load balancing
    int base_chunk_size = n / size;
    int remainder = n % size;
    
    // Calculate local chunk size for each process
    int local_chunk_size = base_chunk_size + (rank < remainder ? 1 : 0);
    
    // Calculate displacement for scatterv
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
    
    // Allocate local array
    int* local_array = (int*)malloc(local_chunk_size * sizeof(int));
    
    // Scatter data to all processes
    MPI_Scatterv(a, send_counts, displacements, MPI_INT, 
                 local_array, local_chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Sort local chunk
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
 * Initialize MPI environment
 */
int initializeMPI(int argc, char* argv[]) {
    int provided;
    int result = MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    
    if (result != MPI_SUCCESS) {
        printf(RED "Error initializing MPI\n" RESET);
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

// Stub implementations when MPI is not available
void parallelInsertionSortMPIAsc(int a[], int n) {
    printf(RED "MPI not available - falling back to sequential sort\n" RESET);
    insertionSortAsc(a, n);
}

void parallelInsertionSortMPIDesc(int a[], int n) {
    printf(RED "MPI not available - falling back to sequential sort\n" RESET);
    insertionSortDesc(a, n);
}

// Demonstration and benchmark stub functions moved to ogt_ui.c

int initializeMPI(int argc, char* argv[]) {
    printf(RED "MPI initialization not available - MPI not compiled\n" RESET);
    return -1;
}

void finalizeMPI(void) {
    printf(RED "MPI finalization not available - MPI not compiled\n" RESET);
}

void getMPIInfo(int* rank, int* size) {
    *rank = 0;
    *size = 1;
    printf(RED "MPI info not available - MPI not compiled\n" RESET);
}

int isMPIInitialized(void) {
    return 0;
}

#endif // HAVE_MPI 