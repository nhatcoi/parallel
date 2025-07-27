# 🚀 Hướng dẫn Build và Run - Sort OGT Library

## 📋 Yêu cầu hệ thống
- **macOS**: 10.15+ (tested on macOS 24.3.0)
- **CPU**: Multi-core processor (tested on Apple M1)
- **RAM**: 4GB+ recommended

## 🛠️ Cài đặt Tools cần thiết (macOS)

### 1. Homebrew (nếu chưa có)
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 2. Compiler và Build Tools
```bash
# Xcode Command Line Tools
xcode-select --install

# CMake
brew install cmake

# GNU Make (optional, nhưng recommended)
brew install make
```

### 3. OpenMP Support
```bash
# OpenMP library for macOS
brew install libomp
```

### 4. MPI Support (optional)
```bash
# OpenMPI for distributed computing
brew install open-mpi
```

### 5. Verification
```bash
# Check installations
gcc --version
cmake --version
mpirun --version  # if MPI installed
echo $HOMEBREW_PREFIX  # should show /opt/homebrew
```

## 🏗️ Build Commands

### Quick Build & Run
```bash
# Build and run immediately
make && make run
```

### Standard Build
```bash
# Create build directory and compile
make build
# hoặc
mkdir build && cd build
cmake .. && make
```

### Clean Build
```bash
# Clean and rebuild everything
make rebuild
```

## 🎮 Run Commands

### Interactive Mode
```bash
# Run main application with menu
make run
# hoặc
./build/parallel_sort
```

### Direct Testing
```bash
# Test individual components
make test-seq      # Sequential sorting
make test-openmp   # OpenMP testing  
make test-pthread  # Pthreads testing
make test-all      # Compare all methods
```

### Benchmarking
```bash
# Quick benchmark
make benchmark

# Comprehensive benchmark (all methods)
make benchmark-all
```

### MPI Commands
```bash
# Run with different MPI process counts
make mpi-1         # 1 process
make mpi-4         # 4 processes
make mpi-8         # 8 processes
make mpi-11        # 11 processes (with oversubscribe)

# MPI testing and benchmark
make mpi-test      # Test multiple process counts
make mpi-benchmark # MPI performance analysis
```

## 📊 Make Targets Reference

### Build Targets
| Command | Description |
|---------|-------------|
| `make` hoặc `make build` | Build project |
| `make clean` | Remove build directory |
| `make rebuild` | Clean + build |
| `make quick` | Build + test-all |

### Test Targets  
| Command | Description |
|---------|-------------|
| `make test-seq` | Test sequential sorting |
| `make test-openmp` | Test OpenMP parallel |
| `make test-pthread` | Test Pthreads manual |
| `make test-all` | Compare all 4 methods |

### Benchmark Targets
| Command | Description |
|---------|-------------|
| `make benchmark` | OpenMP benchmark |
| `make benchmark-all` | **Comprehensive benchmark** |
| `make info` | System information |

### MPI Targets
| Command | Description |
|---------|-------------|
| `make mpi-N` | Run with N processes (N=1,2,3...32) |
| `make mpi-test` | Test multiple process counts |
| `make mpi-benchmark` | MPI performance analysis |

### Utility Targets
| Command | Description |
|---------|-------------|
| `make help` | Show all available commands |
| `make status` | Check project build status |
| `make full-test` | Run complete test suite |

## 🔧 Troubleshooting

### OpenMP Issues
```bash
# If OpenMP not found
export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"
```

### MPI Issues  
```bash
# If mpirun not found
export PATH="/opt/homebrew/bin:$PATH"

# For process count > CPU cores
mpirun --oversubscribe -np 12 ./build/parallel_sort
```

### Build Issues
```bash
# Use GNU Make on macOS
gmake instead of make

# Force clean build
rm -rf build && make rebuild
```

## 📈 Quick Start Example

```bash
# 1. Install dependencies
brew install cmake libomp open-mpi

# 2. Build project
make build

# 3. Run comprehensive benchmark
make benchmark-all

# 4. View results
ls benchmark/  # Check ket_qua_benchmark-v*.txt files
```

## 📁 Output Files

- **Benchmark Results**: `benchmark/ket_qua_benchmark-vN.txt`
- **Executable**: `build/parallel_sort`
- **Library**: `build/libsort_ogt.a`

---

**💡 Tip**: Sử dụng `make help` để xem tất cả commands có sẵn!
