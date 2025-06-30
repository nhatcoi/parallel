#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sort_ogt/include/sort_ogt.h"

int main(int argc, char* argv[]) {
    // Initialize MPI if available and running with mpirun
#ifdef HAVE_MPI
    int mpi_init_result = initializeMPI(argc, argv);
    if (mpi_init_result != 0) {
        printf("Warning: MPI initialization failed, continuing without MPI\n");
    }
#endif

    // Run the main test interface
    overallTestOGT();

    // Finalize MPI if it was initialized
#ifdef HAVE_MPI
    finalizeMPI();
#endif

    return 0;
}