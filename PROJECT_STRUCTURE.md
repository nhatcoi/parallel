# Project Structure - Parallel Insertion Sort

## 📁 Organized Directory Layout

```
prl/
├── 📂 src/                         # Source Code
│   ├── main.c                      # Main demo program
│   ├── benchmark.c                 # Performance benchmark suite
│   └── sort_lib.c                  # Sorting algorithms implementation
│
├── 📂 include/                     # Header Files
│   └── sort_lib.h                  # Function declarations & APIs
│
├── 📂 scripts/                     # Utility Scripts
│   ├── build.sh                    # Build the project
│   ├── run_tests.sh               # Run all tests & benchmarks
│   └── clean.sh                   # Clean build artifacts
│
├── 📂 build/                      # Build Output (auto-generated)
│   ├── bin/                       # Executables
│   │   ├── prl                    # Main demo
│   │   └── prl_benchmark          # Benchmark program
│   └── CMakeFiles/                # CMake build files
│
├── 📄 CMakeLists.txt              # CMake configuration
├── 📄 .gitignore                  # Git ignore rules
├── 📄 README.md                   # Main documentation
├── 📄 PROJECT_STRUCTURE.md        # This file
└── 📄 quickstart.sh              # Quick setup script
```

## 🎯 Benefits of This Structure

### ✅ **Separation of Concerns**
- **src/**: All implementation code
- **include/**: Public APIs and headers
- **scripts/**: Build automation and utilities
- **build/**: Generated files (not in version control)

### ✅ **Professional Standards**
- Follows C/C++ project conventions
- Clear separation between public interface and implementation
- Automated build and test workflows
- Clean git repository (build artifacts ignored)

### ✅ **Easy to Use**
- One-command build: `./scripts/build.sh`
- One-command test: `./scripts/run_tests.sh`
- Clear documentation and examples

### ✅ **Scalable**
- Easy to add new source files to `src/`
- Easy to add new public APIs to `include/`
- Build system automatically handles dependencies

## 🚀 Quick Start

```bash
# First time setup
./quickstart.sh

# Or manual steps:
chmod +x scripts/*.sh    # Make scripts executable
./scripts/build.sh       # Build project
./scripts/run_tests.sh   # Run tests
```

## 📝 File Descriptions

### Source Files (`src/`)

| File | Purpose | Key Functions |
|------|---------|---------------|
| `main.c` | Demo program | Basic sorting demo, thread comparison |
| `benchmark.c` | Performance testing | Comprehensive benchmarks, system analysis |
| `sort_lib.c` | Core algorithms | Sequential & parallel insertion sort |

### Headers (`include/`)

| File | Purpose | Contents |
|------|---------|----------|
| `sort_lib.h` | Public API | Function declarations, shared constants |

### Scripts (`scripts/`)

| Script | Purpose | Usage |
|--------|---------|-------|
| `build.sh` | Build project | `./scripts/build.sh` |
| `run_tests.sh` | Run all tests | `./scripts/run_tests.sh` |
| `clean.sh` | Clean build | `./scripts/clean.sh` |

## 🔧 Build System

### CMake Configuration
- **Target 1**: `prl` - Main demonstration program
- **Target 2**: `prl_benchmark` - Performance analysis
- **Dependencies**: OpenMP, standard C libraries
- **Output**: Executables in `build/bin/`

### Compiler Flags
- **Standard**: C11
- **OpenMP**: Enabled for parallel processing
- **Optimization**: Release mode for benchmarks

## 📊 Usage Patterns

### Development Workflow
```bash
1. Edit source in src/
2. Run ./scripts/build.sh
3. Test with ./scripts/run_tests.sh
4. Commit changes
```

### Adding New Features
```bash
1. Add implementation to src/
2. Add public API to include/
3. Update CMakeLists.txt if needed
4. Test and document
```

### Performance Analysis
```bash
1. Build: ./scripts/build.sh
2. Run: cd build && ./bin/prl_benchmark
3. Analyze results and optimize
```

## 🎛️ Configuration

### Thread Counts
Default test configurations: `1, 3, 5, 7, 9, 11`
- Modify in `src/benchmark.c` or `src/main.c`

### Array Sizes
Default test sizes: `10K, 25K, 50K, 75K, 100K`
- Modify in `src/benchmark.c`

### Build Options
- Debug: `cmake -DCMAKE_BUILD_TYPE=Debug ..`
- Release: `cmake -DCMAKE_BUILD_TYPE=Release ..`

---

**Note**: This organized structure replaced the previous flat layout and provides better maintainability, clarity, and professional development experience. 