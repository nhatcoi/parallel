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
    echo "2. 🧹 Clean Build"
    echo "3. 🚀 Run Program"
    echo "4. ❌ Exit"
    echo ""
}

# Function to execute choice
execute_choice() {
    case $1 in
        1)
            echo "🔨 Building project..."
            mkdir -p build
            cd build
            cmake .. && make
            cd ..
            ;;
        2)
            echo "🧹 Cleaning build..."
            rm -rf build
            echo "✅ Build directory cleaned!"
            ;;
        3)
            echo "🚀 Running program..."
            if [ ! -f "build/bin/prl" ]; then
                echo "Building first..."
                mkdir -p build
                cd build
                cmake .. && make
                cd ..
            fi
            if [ -f "build/bin/prl" ]; then
                ./build/bin/prl
            else
                echo "❌ Build failed! Cannot run program."
            fi
            ;;
        4)
            echo "👋 Goodbye!"
            exit 0
            ;;
        *)
            echo "❌ Invalid option. Please choose 1-4."
            ;;
    esac
}

# Main menu loop
while true; do
    echo ""
    show_menu
    read -p "Choose an option (1-4): " choice
    echo ""
    
    execute_choice $choice
    
    if [ $choice -eq 4 ]; then
        break
    fi
    
    echo ""
    echo "Press Enter to continue..."
    read
done 