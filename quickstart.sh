#!/bin/bash

# Quickstart script for Parallel Insertion Sort project

echo "🚀 PARALLEL INSERTION SORT - QUICKSTART"
echo "======================================"

# Clean old files if they exist
echo "🧹 Cleaning old files..."
rm -f main.c benchmark.c sort_lib.c sort_lib.h prl run_test.sh 2>/dev/null

# Make scripts executable
chmod +x scripts/*.sh

echo "✅ Project structure organized!"
echo ""
echo "📁 Current structure:"
echo "├── src/           - Source code"
echo "├── include/       - Headers"
echo "├── scripts/       - Build & test scripts"
echo "└── build/         - Build output"
echo ""

# Function to show menu
show_menu() {
    echo "🎯 Available Options:"
    echo "===================="
    echo "1. 🔨 Build Project"
    echo "2. 🧪 Run Tests & Benchmarks"
    echo "3. 🧹 Clean Build"
    echo "4. 🔄 Build + Test (Recommended)"
    echo "5. 📊 Quick Demo Only"
    echo "6. 🚀 Full Benchmark Only"
    echo "7. ❌ Exit"
    echo ""
}

# Function to execute choice
execute_choice() {
    case $1 in
        1)
            echo "🔨 Building project..."
            ./scripts/build.sh
            ;;
        2)
            echo "🧪 Running tests and benchmarks..."
            ./scripts/run_tests.sh
            ;;
        3)
            echo "🧹 Cleaning build..."
            ./scripts/clean.sh
            ;;
        4)
            echo "🔄 Building and testing..."
            ./scripts/build.sh && ./scripts/run_tests.sh
            ;;
        5)
            echo "📊 Running quick demo..."
            if [ ! -f "build/bin/prl" ]; then
                echo "Building first..."
                ./scripts/build.sh
            fi
            cd build && ./bin/prl
            cd ..
            ;;
        6)
            echo "🚀 Running full benchmark..."
            if [ ! -f "build/bin/prl_benchmark" ]; then
                echo "Building first..."
                ./scripts/build.sh
            fi
            cd build && ./bin/prl_benchmark
            cd ..
            ;;
        7)
            echo "👋 Goodbye!"
            exit 0
            ;;
        *)
            echo "❌ Invalid option. Please choose 1-7."
            ;;
    esac
}

# Main menu loop
while true; do
    echo ""
    show_menu
    read -p "Choose an option (1-7): " choice
    echo ""
    
    execute_choice $choice
    
    if [ $choice -eq 7 ]; then
        break
    fi
    
    echo ""
    echo "Press Enter to continue..."
    read
done 