cmake_minimum_required(VERSION 3.10)
project(ParallelInsertionSort VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Threads library for Pthreads
find_package(Threads REQUIRED)

# Optional: Find MPI
find_package(MPI)

# Set compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Handle OpenMP on macOS and other systems
if(APPLE)
    # macOS with Homebrew OpenMP
    find_package(OpenMP)
    if(NOT OpenMP_FOUND)
        # Try to find Homebrew OpenMP manually
        set(OpenMP_C_FLAGS "-Xpreprocessor;-fopenmp")
        set(OpenMP_C_INCLUDE_DIRS "/opt/homebrew/opt/libomp/include")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)
        
        # Check if the library exists
        if(EXISTS "${OpenMP_omp_LIBRARY}")
            set(OpenMP_FOUND TRUE)
            add_library(OpenMP::OpenMP_C SHARED IMPORTED)
            set_target_properties(OpenMP::OpenMP_C PROPERTIES
                IMPORTED_LOCATION "${OpenMP_omp_LIBRARY}"
                INTERFACE_COMPILE_OPTIONS "${OpenMP_C_FLAGS}"
                INTERFACE_INCLUDE_DIRECTORIES "${OpenMP_C_INCLUDE_DIRS}")
            message(STATUS "OpenMP found: Using Homebrew libomp")
        else()
            message(WARNING "OpenMP not found - parallel features will be limited")
            set(OpenMP_FOUND FALSE)
        endif()
    endif()
else()
    # Linux and other systems
    find_package(OpenMP REQUIRED)
endif()

# Include directory
include_directories(include)

# Define the source files
set(LIBRARY_SOURCES
    src/sort_seq.c
    src/sort_openmp.c
    src/sort_pthread.c
    src/sort_mpi.c
    src/utils.c
    src/ogt_ui.c
)

# Create the library
add_library(sort_ogt STATIC ${LIBRARY_SOURCES})

# Include directory for library
target_include_directories(sort_ogt PUBLIC include)

# Link OpenMP if available
if(OpenMP_FOUND)
    target_link_libraries(sort_ogt PUBLIC OpenMP::OpenMP_C)
else()
    message(WARNING "Building without OpenMP - parallel features will be disabled")
endif()

# Link Threads library for Pthreads
target_link_libraries(sort_ogt PUBLIC Threads::Threads)

# Link MPI if available
if(MPI_FOUND)
    target_link_libraries(sort_ogt PUBLIC MPI::MPI_C)
    target_compile_definitions(sort_ogt PUBLIC HAVE_MPI)
endif()

# Create the main executable
add_executable(parallel_sort src/main.c)

# Link the library to the executable
target_link_libraries(parallel_sort PRIVATE sort_ogt)

# Link additional libraries for the executable
target_link_libraries(parallel_sort PRIVATE Threads::Threads)

if(OpenMP_FOUND)
    target_link_libraries(parallel_sort PRIVATE OpenMP::OpenMP_C)
endif()

if(MPI_FOUND)
    target_link_libraries(parallel_sort PRIVATE MPI::MPI_C)
endif()

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "OpenMP found: ${OpenMP_FOUND}")
message(STATUS "MPI found: ${MPI_FOUND}")
if(OpenMP_FOUND)
    message(STATUS "OpenMP support enabled")
else()
    message(STATUS "OpenMP support disabled")
endif()
if(MPI_FOUND)
    message(STATUS "MPI support enabled")
else()
    message(STATUS "MPI support disabled (optional)")
endif()

# Installation
install(TARGETS sort_ogt parallel_sort DESTINATION bin)
install(FILES include/sort_ogt.h DESTINATION include)

# ========== CUSTOM TARGETS ==========

# Custom target: make run
add_custom_target(run
    COMMAND ./parallel_sort
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running parallel_sort application"
)

# Create test input files
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_seq_input.txt "1\n1\n0\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_openmp_input.txt "2\n1\n0\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_pthread_input.txt "3\n1\n0\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/test_all_input.txt "5\n0\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/benchmark_input.txt "2\n2\n100000\n0\n")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/info_input.txt "6\n0\n")

# Custom target: make test-seq (test sequential)
add_custom_target(test-seq
    COMMAND ./parallel_sort < test_seq_input.txt
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Testing sequential sorting"
)

# Custom target: make test-openmp (test OpenMP)
add_custom_target(test-openmp
    COMMAND ./parallel_sort < test_openmp_input.txt
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Testing OpenMP sorting"
)

# Custom target: make test-pthread (test Pthreads)
add_custom_target(test-pthread
    COMMAND ./parallel_sort < test_pthread_input.txt
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Testing Pthreads sorting"
)

# Custom target: make test-all (comprehensive test)
add_custom_target(test-all
    COMMAND ./parallel_sort < test_all_input.txt
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running comprehensive comparison test"
)

# Custom target: make benchmark (benchmark with 100K elements)
add_custom_target(benchmark
    COMMAND ./parallel_sort < benchmark_input.txt
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running OpenMP benchmark with 100K elements"
)

# Custom target: make mpi-run (run with 4 MPI processes)
if(MPI_FOUND)
    add_custom_target(mpi-run
        COMMAND mpirun -np 4 ./parallel_sort
        DEPENDS parallel_sort
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running parallel_sort with 4 MPI processes"
    )
    
    # Custom target: make mpi-test (test MPI with different process counts)
    add_custom_target(mpi-test
        COMMAND bash -c "for np in 1 2 4 8; do echo \"Testing with $$np processes:\"; echo \"4\\n2\\n10000\\n0\" | mpirun -np $$np ./parallel_sort; done"
        DEPENDS parallel_sort
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Testing MPI with different process counts"
    )
endif()

# Custom target: make clean-build (clean and rebuild)
add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Clean and rebuild project"
)

# Custom target: make info (show system information)
add_custom_target(info
    COMMAND ./parallel_sort < info_input.txt
    DEPENDS parallel_sort
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Showing system information"
)

# Custom target: make help-targets (show available custom targets)
add_custom_target(help-targets
    COMMAND echo "Available custom targets:"
    COMMAND echo "  make run          - Run the application interactively"
    COMMAND echo "  make test-seq     - Test sequential sorting"
    COMMAND echo "  make test-openmp  - Test OpenMP sorting"
    COMMAND echo "  make test-pthread - Test Pthreads sorting"
    COMMAND echo "  make test-all     - Run comprehensive comparison test"
    COMMAND echo "  make benchmark    - Run OpenMP benchmark with 100K elements"
    COMMAND echo "  make info         - Show system information"
    COMMAND echo "  make clean-build  - Clean and rebuild project"
    COMMAND echo "  make help-targets - Show this help"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Showing available custom targets"
)

# Print build instructions
message(STATUS "")
message(STATUS "Build instructions:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake ..")
message(STATUS "  make")
message(STATUS "  make run")
message(STATUS "")
message(STATUS "Available custom targets:")
message(STATUS "  make run          - Run the application")
message(STATUS "  make test-seq     - Test sequential sorting")
message(STATUS "  make test-openmp  - Test OpenMP sorting")
message(STATUS "  make test-pthread - Test Pthreads sorting")
message(STATUS "  make test-all     - Comprehensive comparison")
message(STATUS "  make benchmark    - OpenMP benchmark")
message(STATUS "  make info         - System information")
message(STATUS "  make help-targets - Show all targets")
message(STATUS "")
if(MPI_FOUND)
    message(STATUS "MPI targets:")
    message(STATUS "  make mpi-run      - Run with 4 MPI processes")
    message(STATUS "  make mpi-test     - Test multiple process counts")
    message(STATUS "")
endif() 