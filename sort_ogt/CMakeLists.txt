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

# Define the source files (now combined into fewer files)
set(SORT_OGT_SOURCES
    src/ogt/sort_seq.c
    src/ogt/sort_openmp.c
    src/ogt/sort_pthread.c
    src/ogt/sort_mpi.c
    src/ogt/utils.c
    src/ogt/ogt_ui.c
)

# Create the library
add_library(sort_ogt STATIC ${SORT_OGT_SOURCES})

# Include directory
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
install(TARGETS sort_ogt DESTINATION lib)
install(FILES include/sort_ogt.h DESTINATION include)
