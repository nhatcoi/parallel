# Makefile wrapper cho Parallel Sorting Project
# NOTE: Requires GNU Make 4.x. On macOS, use 'gmake' instead of 'make'

BUILD_DIR := build
PROJECT_NAME := parallel_sort

# Colors for pretty output
RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
CYAN := \033[36m
RESET := \033[0m

.PHONY: all clean run help test-seq test-openmp test-pthread test-all benchmark info
.PHONY: mpi-1 mpi-2 mpi-3 mpi-4 mpi-5 mpi-6 mpi-7 mpi-8 mpi-9 mpi-10 mpi-11 mpi-12 mpi-16 mpi-32
.PHONY: rebuild quick full-test mpi-test mpi-benchmark status

# Default target
all: build

# Build project
build:
	@echo "$(CYAN)📁 Creating build directory if needed...$(RESET)"
	@mkdir -p $(BUILD_DIR)
	@echo "$(BLUE)⚙️  Configuring project with CMake...$(RESET)"
	@cd $(BUILD_DIR) && cmake ..
	@echo "$(YELLOW)🔨 Building project...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE)
	@echo "$(GREEN)✅ Build completed successfully!$(RESET)"

# Run application
run: build
	@echo "$(GREEN)🚀 Running $(PROJECT_NAME)...$(RESET)"
	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)

# Test commands
test-seq: build
	@echo "$(CYAN)🧪 Testing sequential sorting...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE) test-seq

test-openmp: build
	@echo "$(CYAN)🧪 Testing OpenMP sorting...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE) test-openmp

test-pthread: build
	@echo "$(CYAN)🧪 Testing Pthreads sorting...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE) test-pthread

test-all: build
	@echo "$(CYAN)🧪 Running comprehensive comparison test...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE) test-all

benchmark: build
	@echo "$(YELLOW)📊 Running OpenMP benchmark...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE) benchmark

info: build
	@echo "$(BLUE)ℹ️  Showing system information...$(RESET)"
	@cd $(BUILD_DIR) && $(MAKE) info

# MPI commands - Template
define MPI_TARGET_TEMPLATE
mpi-$(1): build
	@echo "$(GREEN)🌐 Running with $(1) MPI processes...$(RESET)"
	@cd $(BUILD_DIR) && mpirun -np $(1) ./$(PROJECT_NAME)
endef

# Generate MPI targets
$(foreach N,1 2 3 4 5 6 7 8 9 10 11 12 16 32,$(eval $(call MPI_TARGET_TEMPLATE,$(N))))

# MPI tests
mpi-test: build
	@echo "$(YELLOW)🧪 Testing MPI with different process counts...$(RESET)"
	@for np in 1 2 4 8; do \
		echo "$(CYAN)Testing with $$np processes:$(RESET)"; \
		cd $(BUILD_DIR) && echo "5\\n0" | mpirun -np $$np ./$(PROJECT_NAME); \
	done

mpi-benchmark: build
	@echo "$(YELLOW)📊 MPI Benchmark...$(RESET)"
	@for np in 1 2 4 6 8; do \
		echo "$(CYAN)Benchmarking with $$np processes:$(RESET)"; \
		cd $(BUILD_DIR) && echo "4\\n2\\n50000\\n0" | mpirun -np $$np ./$(PROJECT_NAME); \
		echo ""; \
	done

# Utility commands
clean:
	@echo "$(RED)🧹 Cleaning build directory...$(RESET)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✅ Clean completed!$(RESET)"

rebuild: clean build
	@echo "$(GREEN)🔄 Rebuild completed!$(RESET)"

# Quick commands
quick: build test-all

full-test: build test-seq test-openmp test-pthread test-all benchmark info

# Help
help:
	@echo "$(BLUE)🔧 Parallel Sorting Project - Available Commands:$(RESET)"
	@echo "$(YELLOW)⚠️  Use 'gmake' instead of 'make' on macOS$(RESET)"
	@echo ""
	@echo "$(YELLOW)Build Commands:$(RESET)"
	@echo "  make               - Build project (default)"
	@echo "  make build         - Build project"
	@echo "  make clean         - Remove build directory"
	@echo "  make rebuild       - Clean and rebuild"
	@echo ""
	@echo "$(YELLOW)Test Commands:$(RESET)"
	@echo "  make test-seq      - Test sequential sorting"
	@echo "  make test-openmp   - Test OpenMP sorting"
	@echo "  make test-pthread  - Test Pthreads sorting"
	@echo "  make test-all      - Comprehensive comparison test"
	@echo "  make benchmark     - OpenMP benchmark"
	@echo "  make info          - Show system information"
	@echo ""
	@echo "$(YELLOW)MPI Commands:$(RESET)"
	@echo "  make mpi-N         - Run with N MPI processes (N=1,2,3...32)"
	@echo "  make mpi-test      - Test MPI with multiple process counts"
	@echo "  make mpi-benchmark - Benchmark MPI performance"
	@echo ""
	@echo "$(YELLOW)Quick Commands:$(RESET)"
	@echo "  make quick         - Build + test-all"
	@echo "  make full-test     - Run all tests"

status:
	@echo "$(BLUE)📊 Project Status:$(RESET)"
	@echo "Project: $(PROJECT_NAME)"
	@echo "Build Directory: $(BUILD_DIR)"
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "Build Status: $(GREEN)✅ Exists$(RESET)"; \
		if [ -f "$(BUILD_DIR)/$(PROJECT_NAME)" ]; then \
			echo "Executable: $(GREEN)✅ Built$(RESET)"; \
		else \
			echo "Executable: $(RED)❌ Not built$(RESET)"; \
		fi; \
	else \
		echo "Build Status: $(RED)❌ Not created$(RESET)"; \
	fi
	@echo "Available targets: run help clean build test-* mpi-* benchmark info" 