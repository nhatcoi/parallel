#!/bin/bash

# Script Benchmark Tổng Hợp cho Sắp Xếp Song Song
# Test: OpenMP, Pthreads, MPI với nhiều kích thước và số lượng luồng/tiến trình khác nhau

# Định nghĩa màu sắc
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
MAGENTA='\033[35m'
CYAN='\033[36m'
RESET='\033[0m'

# Cấu hình test
ARRAY_SIZES=(1000 5000 10000 30000 50000 100000)
THREAD_COUNTS=(1 3 5 7 9 11)
PROCESS_COUNTS=(1 3 5 7 9 11)
EXECUTABLE="./build/parallel_sort"

# Lưu trữ kết quả (sử dụng mảng indexed thay vì associative)
# Định dạng: kết quả được lưu như "size:threads:time"
openmp_results=()
pthread_results=()
mpi_results=()

# Tạo version incremental và tên file output
# Tìm version cao nhất hiện có và tăng lên 1
LATEST_VERSION=$(ls benchmark/ket_qua_benchmark-v*.txt 2>/dev/null | sed 's/.*-v\([0-9]*\)\.txt/\1/' | sort -n | tail -1)
if [ -z "$LATEST_VERSION" ]; then
    NEW_VERSION=1
else
    NEW_VERSION=$((LATEST_VERSION + 1))
fi
VERSION="v${NEW_VERSION}"
OUTPUT_FILE="benchmark/ket_qua_benchmark-${VERSION}.txt"

# Tạo thư mục benchmark nếu chưa có
mkdir -p benchmark

# Thông báo file output
echo -e "${CYAN}📁 Kết quả sẽ được lưu vào: ${OUTPUT_FILE}${RESET}"

# Redirect output để vừa hiển thị vừa lưu file
exec > >(tee "$OUTPUT_FILE")

echo -e "${MAGENTA}╔══════════════════════════════════════════════════════╗${RESET}"
echo -e "${MAGENTA}║            BỘ CÔNG CỤ BENCHMARK TỔNG HỢP             ║${RESET}"
echo -e "${MAGENTA}║              Phân Tích Sắp Xếp Song Song             ║${RESET}"
echo -e "${MAGENTA}╚══════════════════════════════════════════════════════╝${RESET}"

echo -e "\n${CYAN}📊 Cấu Hình Test:${RESET}"
echo -e "Kích thước mảng: ${ARRAY_SIZES[*]}"
echo -e "Số lượng luồng: ${THREAD_COUNTS[*]}"
echo -e "Số lượng tiến trình: ${PROCESS_COUNTS[*]}"
echo -e "Tổng số test: $((${#ARRAY_SIZES[@]} * ${#THREAD_COUNTS[@]} * 3)) tests"

# Xây dựng project
echo -e "\n${YELLOW}🔨 Đang xây dựng project...${RESET}"
make rebuild > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Xây dựng thất bại!${RESET}"
    exit 1
fi
echo -e "${GREEN}✅ Xây dựng thành công!${RESET}"

# extract_time extracts the average execution time for a specified sorting method from the benchmark output.
extract_time() {
    local method=$1
    local output=$2
    
    # Ánh xạ tên phương pháp từ tiếng Anh sang tiếng Việt  
    case $method in
        "Sequential") local_method="Tuần Tự" ;;
        "OpenMP") local_method="OpenMP" ;;
        "Pthreads") local_method="Pthreads" ;;
        "MPI") local_method="MPI" ;;
        *) local_method="$method" ;;
    esac
    
    # Tìm phương pháp trong bảng so sánh và trích xuất thời gian
    echo "$output" | grep -A 5 "Phương Pháp.*Thời Gian TB" | grep -E "^\s*${local_method}\s*\|" | awk -F'|' '{print $2}' | awk '{print $1}'
}

# get_result retrieves the stored benchmark time for a given method, array size, and thread or process count, or echoes "N/A" if not found.
get_result() {
    local array_name=$1
    local size=$2
    local threads=$3
    local key="${size}:${threads}"
    
    case $array_name in
        "openmp")
            for result in "${openmp_results[@]}"; do
                if [[ $result == ${key}:* ]]; then
                    echo "${result##*:}"
                    return
                fi
            done
            ;;
        "pthread")
            for result in "${pthread_results[@]}"; do
                if [[ $result == ${key}:* ]]; then
                    echo "${result##*:}"
                    return
                fi
            done
            ;;
        "mpi")
            for result in "${mpi_results[@]}"; do
                if [[ $result == ${key}:* ]]; then
                    echo "${result##*:}"
                    return
                fi
            done
            ;;
    esac
    echo "N/A"
}

# run_openmp_benchmark runs the OpenMP benchmark for a specified array size and thread count, extracts the average execution time, and stores the result for later analysis.
run_openmp_benchmark() {
    local size=$1
    local threads=$2
    
    echo -e "${BLUE}  Đang test: OpenMP - Kích thước: ${size}, Luồng: ${threads}${RESET}"
    
    # Chạy benchmark OpenMP cụ thể (tùy chọn 2, tùy chọn phụ 2)
    local output=$(echo -e "2\n2\n${size}\n${threads}\n0\n0\n0" | ${EXECUTABLE} 2>/dev/null)
    
    if [ $? -eq 0 ] && [ -n "$output" ]; then
        # Trích xuất thời gian OpenMP cho số luồng cụ thể
        local openmp_time=$(echo "$output" | grep -A 10 "Luồng.*Thời Gian TB" | grep -E "^\s*${threads}\s*\|" | awk -F'|' '{print $2}' | awk '{print $1}')
        openmp_results+=("${size}:${threads}:${openmp_time}")
        echo -e "    ${GREEN}✓${RESET} OpenMP: ${openmp_time}s"
    else
        echo -e "    ${RED}✗ Test OpenMP thất bại${RESET}"
        openmp_results+=("${size}:${threads}:FAIL")
    fi
}

# run_pthread_benchmark runs the Pthreads benchmark for a specified array size and thread count, extracts the average execution time, and stores the result for later analysis.
run_pthread_benchmark() {
    local size=$1
    local threads=$2
    
    echo -e "${BLUE}  Đang test: Pthreads - Kích thước: ${size}, Luồng: ${threads}${RESET}"
    
    # Chạy benchmark Pthreads cụ thể (tùy chọn 3, tùy chọn phụ 2)
    local output=$(echo -e "3\n2\n${size}\n${threads}\n0\n0\n0" | ${EXECUTABLE} 2>/dev/null)
    
    if [ $? -eq 0 ] && [ -n "$output" ]; then
        # Trích xuất thời gian Pthreads cho số luồng cụ thể
        local pthread_time=$(echo "$output" | grep -A 10 "Luồng.*Thời Gian TB" | grep -E "^\s*${threads}\s*\|" | awk -F'|' '{print $2}' | awk '{print $1}')
        pthread_results+=("${size}:${threads}:${pthread_time}")
        echo -e "    ${GREEN}✓${RESET} Pthreads: ${pthread_time}s"
    else
        echo -e "    ${RED}✗ Test Pthreads thất bại${RESET}"
        pthread_results+=("${size}:${threads}:FAIL")
    fi
}

# run_mpi_benchmark runs the MPI benchmark for a specified array size and number of processes, extracts the average execution time, and stores the result.
run_mpi_benchmark() {
    local size=$1
    local processes=$2
    
    echo -e "${YELLOW}  Đang test: MPI - Kích thước: ${size}, Tiến trình: ${processes}${RESET}"
    
    # Chạy benchmark MPI cụ thể với oversubscribe nếu cần
    local mpi_cmd="mpirun"
    if [ ${processes} -gt 10 ]; then
        mpi_cmd="mpirun --oversubscribe"
        echo -e "    ${YELLOW}⚠️  Sử dụng oversubscribe cho ${processes} > 10 cores${RESET}"
    fi
    
    local output=$(echo -e "4\n2\n${size}\n0\n0\n0" | ${mpi_cmd} -np ${processes} ${EXECUTABLE} 2>/dev/null)
    
    if [ $? -eq 0 ] && [ -n "$output" ]; then
        # Trích xuất thời gian MPI cho số tiến trình cụ thể
        local mpi_time=$(echo "$output" | grep -A 5 "Tiến Trình.*Thời Gian TB" | grep -E "^\s*${processes}\s*\|" | awk -F'|' '{print $2}' | awk '{print $1}')
        mpi_results+=("${size}:${processes}:${mpi_time}")
        echo -e "    ${GREEN}✓${RESET} MPI: ${mpi_time}s"
    else
        echo -e "    ${RED}✗ Test MPI thất bại${RESET}"
        mpi_results+=("${size}:${processes}:FAIL")
    fi
}

# Thực thi benchmark chính
echo -e "\n${CYAN}🚀 Bắt đầu benchmark toàn diện...${RESET}"

total_tests=0
completed_tests=0

# Test OpenMP (single process, multiple threads)
echo -e "\n${MAGENTA}=== OPENMP BENCHMARK ===${RESET}"
for size in "${ARRAY_SIZES[@]}"; do
    for threads in "${THREAD_COUNTS[@]}"; do
        ((total_tests++))
        run_openmp_benchmark $size $threads
        ((completed_tests++))
        echo -e "${CYAN}Tiến độ: ${completed_tests}/${total_tests}${RESET}"
    done
done

# Test Pthreads (đơn tiến trình, đa luồng)
echo -e "\n${MAGENTA}=== BENCHMARK PTHREADS ===${RESET}"
for size in "${ARRAY_SIZES[@]}"; do
    for threads in "${THREAD_COUNTS[@]}"; do
        ((total_tests++))
        run_pthread_benchmark $size $threads
        ((completed_tests++))
        echo -e "${CYAN}Tiến độ: ${completed_tests}/${total_tests}${RESET}"
    done
done

# Test MPI (đa tiến trình)
echo -e "\n${MAGENTA}=== BENCHMARK MPI ===${RESET}"
for size in "${ARRAY_SIZES[@]}"; do
    for processes in "${PROCESS_COUNTS[@]}"; do
        run_mpi_benchmark $size $processes
    done
done

# Tạo bảng kết quả tổng hợp
echo -e "\n${MAGENTA}╔══════════════════════════════════════════════════════╗${RESET}"
echo -e "${MAGENTA}║                 KẾT QUẢ TỔNG HỢP                    ║${RESET}"
echo -e "${MAGENTA}╚══════════════════════════════════════════════════════╝${RESET}"

echo -e "\n${CYAN}📊 KẾT QUẢ THỜI GIAN CHI TIẾT (giây)${RESET}"
echo -e "\n${YELLOW}🔥 Kết Quả Kết Hợp (Count = Luồng cho OpenMP/Pthreads, Tiến trình cho MPI):${RESET}"
printf "%-10s | %-8s | %-12s | %-12s | %-12s\n" "Kích Thước" "Số Lượng" "OpenMP" "Pthreads" "MPI"
echo "---------------------------------------------------------------"

for size in "${ARRAY_SIZES[@]}"; do
    for count in "${THREAD_COUNTS[@]}"; do
        openmp_time=$(get_result "openmp" "$size" "$count")
        pthread_time=$(get_result "pthread" "$size" "$count")
        mpi_time=$(get_result "mpi" "$size" "$count")
        
        printf "%-10s | %-8s | %-12s | %-12s | %-12s\n" \
            "$size" "$count" "$openmp_time" "$pthread_time" "$mpi_time"
    done
    echo "---------------------------------------------------------------"
done

# Tính toán và hiển thị thống kê tóm tắt
echo -e "\n${CYAN}📈 TÓM TẮT HIỆU SUẤT${RESET}"
echo -e "\n${YELLOW}Hiệu Suất Tốt Nhất Theo Kích Thước Mảng:${RESET}"

for size in "${ARRAY_SIZES[@]}"; do
    echo -e "\n${GREEN}Kích Thước Mảng: ${size} phần tử${RESET}"
    
    best_openmp="N/A"
    best_pthread="N/A" 
    best_mpi="N/A"
    best_openmp_time=999999
    best_pthread_time=999999
    best_mpi_time=999999
    
    # Tìm OpenMP và Pthreads tốt nhất (theo luồng)
    for threads in "${THREAD_COUNTS[@]}"; do
        openmp_time=$(get_result "openmp" "$size" "$threads")
        pthread_time=$(get_result "pthread" "$size" "$threads")
        
        if [[ "$openmp_time" != "FAIL" && "$openmp_time" != "N/A" && "$openmp_time" != "" ]]; then
            if (( $(echo "$openmp_time < $best_openmp_time" | bc -l 2>/dev/null || echo 0) )); then
                best_openmp_time=$openmp_time
                best_openmp="${openmp_time}s (${threads} luồng)"
            fi
        fi
        
        if [[ "$pthread_time" != "FAIL" && "$pthread_time" != "N/A" && "$pthread_time" != "" ]]; then
            if (( $(echo "$pthread_time < $best_pthread_time" | bc -l 2>/dev/null || echo 0) )); then
                best_pthread_time=$pthread_time
                best_pthread="${pthread_time}s (${threads} luồng)"
            fi
        fi
    done
    
    # Tìm MPI tốt nhất (theo tiến trình)
    for processes in "${PROCESS_COUNTS[@]}"; do
        mpi_time=$(get_result "mpi" "$size" "$processes")
        
        if [[ "$mpi_time" != "FAIL" && "$mpi_time" != "N/A" && "$mpi_time" != "" ]]; then
            if (( $(echo "$mpi_time < $best_mpi_time" | bc -l 2>/dev/null || echo 0) )); then
                best_mpi_time=$mpi_time
                best_mpi="${mpi_time}s (${processes} tiến trình)"
            fi
        fi
    done
    
    echo "  🚀 OpenMP tốt nhất:   $best_openmp"
    echo "  🧵 Pthreads tốt nhất: $best_pthread" 
    echo "  🌐 MPI tốt nhất:      $best_mpi"
    
    # Xác định tổng thể tốt nhất cho kích thước mảng này
    overall_best_for_size=""
    overall_best_time_for_size=999999
    
    if [[ "$best_openmp_time" != "999999" && "$best_openmp_time" != "" ]] && (( $(echo "$best_openmp_time < $overall_best_time_for_size" | bc -l 2>/dev/null || echo 0) )); then
        overall_best_time_for_size=$best_openmp_time
        overall_best_for_size="OpenMP ($best_openmp)"
    fi
    
    if [[ "$best_pthread_time" != "999999" && "$best_pthread_time" != "" ]] && (( $(echo "$best_pthread_time < $overall_best_time_for_size" | bc -l 2>/dev/null || echo 0) )); then
        overall_best_time_for_size=$best_pthread_time
        overall_best_for_size="Pthreads ($best_pthread)"
    fi
    
    if [[ "$best_mpi_time" != "999999" && "$best_mpi_time" != "" ]] && (( $(echo "$best_mpi_time < $overall_best_time_for_size" | bc -l 2>/dev/null || echo 0) )); then
        overall_best_time_for_size=$best_mpi_time
        overall_best_for_size="MPI ($best_mpi)"
    fi
    
    if [[ "$overall_best_for_size" != "" ]]; then
        echo -e "  ${GREEN}→ Lựa chọn tốt nhất: $overall_best_for_size${RESET}"
    else
        echo "  → Lựa chọn tốt nhất: N/A"
    fi
done

echo -e "\n${MAGENTA}🏆 NHẤT TOÀN DIỆN${RESET}"
# Tìm tốt nhất tuyệt đối trong tất cả các test
overall_best_time=999999
overall_best_method=""
overall_best_config=""

# Kiểm tra OpenMP và Pthreads (theo luồng)
for size in "${ARRAY_SIZES[@]}"; do
    for threads in "${THREAD_COUNTS[@]}"; do
        for method in "openmp" "pthread"; do
            time_value=$(get_result "$method" "$size" "$threads")
            
            if [[ "$time_value" != "FAIL" && "$time_value" != "N/A" && "$time_value" != "" ]]; then
                if (( $(echo "$time_value < $overall_best_time" | bc -l 2>/dev/null || echo 0) )); then
                    overall_best_time=$time_value
                    overall_best_method=$(echo $method | tr '[:lower:]' '[:upper:]')
                    overall_best_config="Kích thước: ${size}, Luồng: ${threads}"
                fi
            fi
        done
    done
done

# Kiểm tra MPI (theo tiến trình)
for size in "${ARRAY_SIZES[@]}"; do
    for processes in "${PROCESS_COUNTS[@]}"; do
        time_value=$(get_result "mpi" "$size" "$processes")
        
        if [[ "$time_value" != "FAIL" && "$time_value" != "N/A" && "$time_value" != "" ]]; then
            if (( $(echo "$time_value < $overall_best_time" | bc -l 2>/dev/null || echo 0) )); then
                overall_best_time=$time_value
                overall_best_method="MPI"
                overall_best_config="Kích thước: ${size}, Tiến trình: ${processes}"
            fi
        fi
    done
done

echo -e "${GREEN}🥇 ${overall_best_method}: ${overall_best_time}s${RESET}"
echo -e "${GREEN}   ${overall_best_config}${RESET}"

echo -e "\n${CYAN}📋 Test hoàn thành! Kết quả đã lưu ở trên.${RESET}"
echo -e "${GREEN}💾 Kết quả đã được tự động lưu vào: ${OUTPUT_FILE}${RESET}"
echo -e "${YELLOW}💡 Để xem kết quả: cat ${OUTPUT_FILE}${RESET}"
echo -e "${YELLOW}📊 Thư mục benchmark: ls -la benchmark/${RESET}" 