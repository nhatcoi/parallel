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
echo "├── prl/           - Library source code"
echo "│   ├── src/       - Library implementation"
echo "│   ├── include/   - Library headers"
echo "│   └── build/     - Library build output"
echo "├── main.c         - Main application"
echo "└── build_main.sh  - Build script"
echo ""

# Function to show menu
show_menu() {
    echo "🎯 Available Options:"
    echo "===================="
    echo "1. 🔨 Build Library Only"
    echo "2. 🚀 Build Library + Main"
    echo "3. 🧹 Clean Build"
    echo "4. 🏃 Run Program"
    echo "5. ❌ Exit"
    echo ""
}

# Function to execute choice
execute_choice() {
    case $1 in
        1)
            echo "🔨 Building library only..."
            mkdir -p build
            cd build
            cmake .. && make
            cd ..
            echo "✅ Library built: build/libsort_ogt.a"
            ;;
        2)
            echo "🚀 Building library + main application..."
            cd ..
            ./build_main.sh
            cd prl
            ;;
        3)
            echo "🧹 Cleaning build..."
            rm -rf build
            cd ..
            rm -f main
            cd prl
            echo "✅ Build directories cleaned!"
            ;;
        4)
            echo "🏃 Running program..."
            cd ..
            if [ ! -f "main" ]; then
                echo "Building first..."
                ./build_main.sh
            fi
            if [ -f "main" ]; then
                ./main
            else
                echo "❌ Build failed! Cannot run program."
            fi
            cd prl
            ;;
        5)
            echo "👋 Goodbye!"
            exit 0
            ;;
        *)
            echo "❌ Invalid option. Please choose 1-5."
            ;;
    esac
}

# Main menu loop
while true; do
    echo ""
    show_menu
    read -p "Choose an option (1-5): " choice
    echo ""
    
    execute_choice $choice
    
    if [ $choice -eq 5 ]; then
        break
    fi
    
    echo ""
    echo "Press Enter to continue..."
    read
done 