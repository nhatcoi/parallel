# 🚀 Parallel Insertion Sort

Thư viện sắp xếp chèn song song được cài đặt bằng **OpenMP**, **Pthreads**, và **MPI**.

## ⚡ Tính Năng Chính

- 🔢 **Sequential Sort**: Thuật toán sắp xếp chèn tuần tự
- 🚀 **OpenMP**: Song song hóa shared memory 
- 🧵 **Pthreads**: Song song hóa với POSIX threads
- 🌐 **MPI**: Song song hóa distributed memory
- 📊 **Benchmark**: So sánh hiệu suất tự động

## 🛠️ Yêu Cầu Hệ Thống

```bash
# Bắt buộc
- C99 compiler (GCC/Clang)
- CMake 3.10+
- Pthreads

# Tùy chọn
- OpenMP library
- MPI implementation
```

## 🚀 Cài Đặt & Chạy

### Build với CMake
```bash
mkdir build && cd build
cmake ..
make
./parallel_sort
```

### Build với Makefile
```bash
make run              # Build + chạy
make benchmark-all    # Benchmark comprehensive

make help             # make support
```

### Chạy với MPI
```bash
make mpi-N            # Số process N
```


## 📁 Cấu Trúc Dự Án

```
src/
├── main.c           # Entry point
├── sort_seq.c       # Sequential implementation  
├── sort_openmp.c    # OpenMP implementation
├── sort_pthread.c   # Pthreads implementation
├── sort_mpi.c       # MPI implementation
├── ogt_ui.c         # Interactive UI
└── utils.c          # Utility functions

include/
└── sort_ogt.h       # Header file (API)

benchmark/           # Benchmark results
```



## 🔧 Platform Support

- ✅ **Linux**: Full support (GCC + OpenMP + MPI)
- ✅ **macOS**: Homebrew OpenMP required (`brew install libomp open-mpi`)
- ✅ **Windows**: MinGW/MSVC support
