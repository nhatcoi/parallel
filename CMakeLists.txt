cmake_minimum_required(VERSION 3.31)
project(prl C)

set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(include)

# macOS specific OpenMP configuration
if(APPLE)
    set(CMAKE_C_COMPILER clang)
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/libomp/lib -lomp")
else()
    find_package(OpenMP REQUIRED)
endif()

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SORT_LIB_SOURCES src/sort_lib.c)

# Main executable
add_executable(prl src/main.c ${SORT_LIB_SOURCES})

# Performance benchmark executable
add_executable(prl_benchmark src/benchmark.c ${SORT_LIB_SOURCES})

# Link OpenMP libraries
if(APPLE)
    target_link_libraries(prl /opt/homebrew/opt/libomp/lib/libomp.dylib)
    target_link_libraries(prl_benchmark /opt/homebrew/opt/libomp/lib/libomp.dylib)
else()
    target_link_libraries(prl OpenMP::OpenMP_C)
    target_link_libraries(prl_benchmark OpenMP::OpenMP_C)
endif()
