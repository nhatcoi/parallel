# Sort OGT Library - Parallel Insertion Sort

🚀 **Professional Parallel Insertion Sort Library** với OpenMP implementation và comprehensive testing suite.

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)]()
[![Language](https://img.shields.io/badge/language-C-orange.svg)]()
[![OpenMP](https://img.shields.io/badge/OpenMP-supported-red.svg)]()

## 📋 Tổng quan

Sort OGT Library là implementation chuyên nghiệp của thuật toán parallel insertion sort, được thiết kế để nghiên cứu và đánh giá hiệu năng với các cấu hình threads khác nhau. Library cung cấp giao diện thân thiện và comprehensive testing suite.

### ✨ Tính năng nổi bật

- 🔄 **Sequential & Parallel Sorting**: Cả hai phiên bản tối ưu
- 📊 **Performance Benchmarking**: Đánh giá hiệu năng chi tiết
- 🧪 **Interactive Testing**: Menu test tương tác đầy đủ
- 🎨 **Colorful UI**: Giao diện màu sắc dễ nhìn
- 📈 **Thread Scaling Analysis**: Phân tích scaling với số threads
- ✅ **Correctness Verification**: Kiểm tra tính đúng đắn
- 🎯 **Modular Design**: Cấu trúc module rõ ràng

## 📁 Cấu trúc Project

```
prl/
├── 📂 src/                     # Source code
│   ├── main.c                  # Entry point chính
│   └── 📂 ogt/                 # OGT Library modules
│       ├── sort_seq.c          # Sequential sorting algorithms
│       ├── sort_openmp.c       # Parallel OpenMP algorithms
│       ├── utils.c             # Utility functions
│       ├── ogt_ui.c            # User interface
│       ├── ogt_test.c          # Testing functions
│       ├── ogt_benchmark.c     # Benchmark suite
│       └── ogt_demo.c          # Demo functions
├── 📂 include/                 # Header files
│   └── sort_ogt.h              # Complete API definitions
├── 📂 build/                   # Build directory (auto-generated)
│   └── bin/                    # Executables
├── CMakeLists.txt              # CMake build configuration
├── quickstart.sh               # Quick build & run script
├── .gitignore                  # Git ignore rules
└── README.md                   # This documentation
```

## 🚀 Quick Start

### Cách nhanh nhất:

```bash
# Clone repository
git clone <repository-url>
cd prl

# Build & run với quickstart script
./quickstart.sh
```

### Build thủ công:

```bash
# Build project
mkdir -p build && cd build
cmake ..
make

# Run program
./bin/prl
```

## 📋 System Requirements

- **OS**: macOS, Linux, Windows (WSL)
- **Compiler**: GCC 7+ hoặc Clang 8+ với OpenMP support
- **CMake**: Version 3.31+
- **OpenMP**: libomp (macOS) hoặc gomp (Linux)

### 🔧 Cài đặt Dependencies

**macOS (Homebrew):**
```bash
brew install cmake libomp
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install cmake gcc libomp-dev
```

**CentOS/RHEL:**
```bash
sudo yum install cmake gcc openmp-devel
```

## 🎮 Sử dụng

### Interactive Menu

Chạy program sẽ hiển thị menu tương tác:

```
=== SORT OGT LIBRARY - MAIN TESTING MENU ===
1. 🔤 Demo Cơ Bản (Basic Sorting Demo)
2. ⚡ Demo Song Song Nhanh (Quick Parallel Demo)
3. 🧪 Test Tuần Tự - Nhập Mảng (Sequential User Input)
4. 🎲 Test Song Song - Mảng Ngẫu Nhiên (Parallel Random Array)
5. 📊 So Sánh Performance Threads (Thread Performance)
6. 🔬 Menu Test Tương Tác Đầy Đủ (Full Interactive Tests)
7. 📈 Benchmark Đơn (Single Array Size)
8. 📊 Benchmark Đầy Đủ (Complete Benchmark Suite)
9. ✅ Kiểm Tra Tính Đúng (Correctness Verification)
10. ℹ️  Thông Tin Hệ Thống (System Information)
11. 📋 Thông Tin Thư Viện (Library Information)
0. 🚪 Thoát (Exit)
```

### API Usage

```c
#include "sort_ogt.h"

int main() {
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr)/sizeof(arr[0]);
    
    // Sequential sort
    insertionSortAsc(arr, n);
    
    // Parallel sort with 4 threads
    parallelInsertionSortAsc(arr, n, 4);
    
    return 0;
}
```

## 📊 Performance Results

### Test Configuration
- **Array Sizes**: 10K, 25K, 50K, 75K, 100K elements
- **Thread Counts**: 1, 3, 5, 7, 9, 11 threads
- **System**: MacBook Pro M1 (10 cores)

### Sample Results

| Array Size | Sequential | 3 Threads | 5 Threads | 7 Threads | 9 Threads | 11 Threads |
|------------|------------|-----------|-----------|-----------|-----------|------------|
| 10K        | 0.025s     | 0.012s    | 0.008s    | 0.006s    | 0.005s    | 0.004s     |
| 50K        | 0.632s     | 0.180s    | 0.089s    | 0.055s    | 0.041s    | 0.035s     |
| 100K       | 2.735s     | 0.344s    | 0.157s    | 0.096s    | 0.067s    | 0.054s     |

**Best Speedup**: 50.53x với 100K elements và 11 threads

## 🧠 Algorithm Implementation

### Parallel Divide-and-Conquer Approach:

```c
void parallelInsertionSortAsc(int a[], int n, int num_threads) {
    omp_set_num_threads(num_threads);
    
    // 1. DIVIDE: Calculate chunk sizes
    int chunk_size = n / num_threads;
    int remainder = n % num_threads;
    
    // 2. CONQUER: Sort chunks in parallel
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        // Sort local chunk using sequential insertion sort
        insertionSortAsc(local_chunk, chunk_length);
    }
    
    // 3. MERGE: Combine sorted chunks
    merge_sorted_chunks(a, chunk_info, num_threads, n);
}
```

### Key Optimizations:
- **Load Balancing**: Even distribution of elements
- **Minimal Overhead**: Single parallel region
- **Efficient Merging**: K-way merge for sorted chunks
- **Memory Management**: Optimized temporary array usage

## 🔧 Build Configuration

### CMake Features:
- **Cross-platform**: macOS, Linux, Windows support
- **OpenMP Detection**: Automatic library detection
- **Release Optimization**: -O2 optimization flags
- **Modular Compilation**: Separate compilation units

### Build Options:
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (default)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Specify OpenMP path (if needed)
cmake -DOpenMP_ROOT=/opt/homebrew/opt/libomp ..
```

## 🧪 Testing & Verification

### Comprehensive Test Suite:
1. **Correctness Tests**: Verify sorting accuracy
2. **Performance Tests**: Measure execution time
3. **Thread Scaling**: Analyze speedup patterns
4. **Edge Cases**: Empty arrays, single elements
5. **Large Data**: Memory stress testing

### Automated Testing:
```bash
./quickstart.sh
# Choose option 9: Correctness Verification
```

## 🚨 Troubleshooting

### Common Issues:

**OpenMP Not Found (macOS):**
```bash
# Install libomp
brew install libomp

# Set environment variables
export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"
```

**Build Errors:**
```bash
# Check CMake version
cmake --version  # Should be 3.31+

# Clean build
rm -rf build && mkdir build && cd build && cmake .. && make
```

**Runtime Issues:**
- Ensure sufficient RAM for large arrays
- Check CPU core count: `nproc` (Linux) or `sysctl -n hw.ncpu` (macOS)
- Monitor system load during benchmarks

## 📖 Documentation

### Function Reference:

**Sequential Sorting:**
- `insertionSortAsc(int a[], int n)` - Ascending order
- `insertionSortDesc(int a[], int n)` - Descending order

**Parallel Sorting:**
- `parallelInsertionSortAsc(int a[], int n, int threads)` - Parallel ascending
- `parallelInsertionSortDesc(int a[], int n, int threads)` - Parallel descending

**Utilities:**
- `getCurrentTime()` - High-precision timing
- `generateRandomArray()` - Test data generation
- `copyArray()` - Array duplication

**Testing & Benchmarking:**
- `runCompleteBenchmark()` - Full performance suite
- `demonstrateSortingCorrectness()` - Correctness verification
- `overallTestOGT()` - Interactive testing menu

## 🤝 Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit changes (`git commit -m 'Add AmazingFeature'`)
4. Push to branch (`git push origin feature/AmazingFeature`)
5. Open Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- **OGT Team** - *Initial work and implementation*

## 🙏 Acknowledgments

- OpenMP Community for parallel programming standards
- CMake developers for cross-platform build system
- Contributors and testers

---

**Happy Sorting!** 🎉
