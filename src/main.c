#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sort_ogt.h"

/**
 * Entry point for the program, handling optional MPI initialization and finalization, and running the main test suite.
 *
 * Initializes MPI if available, executes the primary test routine, and finalizes MPI if it was initialized. Continues execution without MPI if initialization fails.
 * @returns 0 on successful program termination.
 */
int main(int argc, char* argv[]) {
    // Khởi tạo MPI nếu có và chạy với mpirun
#ifdef HAVE_MPI
    int mpi_init_result = initializeMPI(argc, argv);
    if (mpi_init_result != 0) {
        printf("Cảnh báo: Khởi tạo MPI thất bại, tiếp tục không dùng MPI\n");
    }
#endif

    // Chạy trình test chính
    overallTestOGT();

    // Đóng MPI nếu đã khởi tạo
#ifdef HAVE_MPI
    finalizeMPI();
#endif

    return 0;
}