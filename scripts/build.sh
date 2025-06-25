#!/bin/bash

# Build script for Parallel Insertion Sort project

set -e  # Exit on any error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "🔨 Building Parallel Insertion Sort Project"
echo "Project root: $PROJECT_ROOT"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
echo "📋 Configuring with CMake..."
cmake ..

# Build the project
echo "🚀 Building project..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "✅ Build completed successfully!"
echo "Executables are in: $BUILD_DIR/bin/"
echo ""
echo "Available programs:"
echo "  • $BUILD_DIR/bin/prl          - Main demonstration"
echo "  • $BUILD_DIR/bin/prl_benchmark - Performance benchmark"
echo ""
echo "To run: cd build && ./bin/prl" 