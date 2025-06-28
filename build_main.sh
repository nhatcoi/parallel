#!/bin/bash

echo "🏗️  BUILDING SORT_OGT LIBRARY & MAIN APPLICATION"
echo "=============================================="

# Step 1: Build the library
echo "📚 Building sort_ogt library..."
mkdir -p sort_ogt/build
cd sort_ogt/build

# Configure and build the library
cmake .. && make

if [ $? -eq 0 ]; then
    echo "✅ Library built successfully!"
else
    echo "❌ Library build failed!"
    exit 1
fi

# Go back to project root
cd ../..

# Step 2: Compile main.c with the library
echo ""
echo "🔨 Compiling main.c with sort_ogt library..."

# Find required flags from CMake
CMAKE_BUILD_TYPE="Release"
OPENMP_FLAGS=""
MPI_FLAGS=""

# Check if OpenMP is available (for macOS with Homebrew)
if [ -f "/opt/homebrew/opt/libomp/lib/libomp.dylib" ]; then
    OPENMP_FLAGS="-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp"
    echo "🔧 OpenMP support detected (Homebrew)"
elif command -v pkg-config >/dev/null 2>&1 && pkg-config --exists openmp; then
    OPENMP_FLAGS="$(pkg-config --cflags --libs openmp)"
    echo "🔧 OpenMP support detected (pkg-config)"
else
    echo "⚠️  OpenMP not found - compiling without OpenMP"
fi

# Check if MPI is available
if command -v mpicc >/dev/null 2>&1; then
    CC="mpicc"
    MPI_FLAGS="-DHAVE_MPI"
    echo "🔧 MPI support detected"
else
    CC="gcc"
    echo "⚠️  MPI not found - compiling without MPI"
fi

# Compile main.c
$CC -std=c99 -O3 -Isort_ogt/include $MPI_FLAGS $OPENMP_FLAGS \
    main.c -Lsort_ogt/build -lsort_ogt -lpthread -o main

if [ $? -eq 0 ]; then
    echo "✅ main compiled successfully!"
    echo ""
    echo "🚀 Usage:"
    echo "   ./main              # Run the application"
    echo "   mpirun -np 4 ./main # Run with MPI (if available)"
else
    echo "❌ main compilation failed!"
    exit 1
fi

echo ""
echo "🎯 Build completed successfully!"
echo "📁 Files created:"
echo "   - sort_ogt/build/libsort_ogt.a  # Static library"
echo "   - main                     # Main executable" 