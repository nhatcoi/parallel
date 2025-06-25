#!/bin/bash

# Clean script for Parallel Insertion Sort project

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "🧹 Cleaning Parallel Insertion Sort Project"
echo "=========================================="

if [ -d "$BUILD_DIR" ]; then
    echo "🗑️  Removing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR"
    echo "✅ Build directory cleaned!"
else
    echo "ℹ️  No build directory found, nothing to clean."
fi

# Remove any executables in root
if [ -f "$PROJECT_ROOT/prl" ]; then
    echo "🗑️  Removing executable: prl"
    rm -f "$PROJECT_ROOT/prl"
fi

if [ -f "$PROJECT_ROOT/prl_benchmark" ]; then
    echo "🗑️  Removing executable: prl_benchmark"
    rm -f "$PROJECT_ROOT/prl_benchmark"
fi

echo ""
echo "✨ Project cleaned successfully!"
echo "Run './scripts/build.sh' to rebuild the project." 