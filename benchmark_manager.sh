#!/bin/bash

# Script quản lý benchmark results
# Tiện ích để xem, so sánh và quản lý các file kết quả benchmark

# Định nghĩa màu sắc
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
MAGENTA='\033[35m'
CYAN='\033[36m'
RESET='\033[0m'

BENCHMARK_DIR="benchmark"

show_help() {
    echo -e "${CYAN}🔧 BENCHMARK MANAGER - Công cụ quản lý kết quả benchmark${RESET}"
    echo -e "${CYAN}=================================================${RESET}"
    echo -e "\nSử dụng: $0 [option]"
    echo -e "\nCác tùy chọn:"
    echo -e "  ${GREEN}list${RESET}     - Liệt kê tất cả file benchmark"
    echo -e "  ${GREEN}latest${RESET}   - Xem file benchmark mới nhất"
    echo -e "  ${GREEN}view${RESET}     - Xem file benchmark cụ thể"
    echo -e "  ${GREEN}compare${RESET}  - So sánh 2 file benchmark"
    echo -e "  ${GREEN}clean${RESET}    - Dọn dẹp file cũ (giữ lại 5 file gần nhất)"
    echo -e "  ${GREEN}stats${RESET}    - Thống kê tổng quan"
    echo -e "  ${GREEN}help${RESET}     - Hiển thị hướng dẫn này"
}

list_benchmarks() {
    echo -e "${CYAN}📁 Danh sách file benchmark:${RESET}"
    if [ ! -d "$BENCHMARK_DIR" ] || [ -z "$(ls -A $BENCHMARK_DIR 2>/dev/null)" ]; then
        echo -e "${YELLOW}⚠️  Chưa có file benchmark nào.${RESET}"
        return
    fi
    
    echo -e "${BLUE}Sắp xếp theo version (cao nhất trước):${RESET}"
    ls $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | sort -V -r | while read file; do
        size=$(ls -l "$file" | awk '{print $5}')
        date=$(ls -l "$file" | awk '{print $6 " " $7 " " $8}')
        echo -e "  📊 $(basename "$file") (${size} bytes, $date)"
    done
}

view_latest() {
    echo -e "${CYAN}📄 File benchmark mới nhất:${RESET}"
    local latest_file=$(ls $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | sort -V | tail -1)
    
    if [ -z "$latest_file" ]; then
        echo -e "${RED}❌ Không tìm thấy file benchmark nào.${RESET}"
        return
    fi
    
    echo -e "${GREEN}File: $(basename "$latest_file")${RESET}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RESET}"
    cat "$latest_file"
}

view_specific() {
    echo -e "${CYAN}📄 Chọn file để xem:${RESET}"
    
    if [ ! -d "$BENCHMARK_DIR" ]; then
        echo -e "${RED}❌ Thư mục benchmark không tồn tại.${RESET}"
        return
    fi
    
    local files=($(ls $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | sort -V -r))
    
    if [ ${#files[@]} -eq 0 ]; then
        echo -e "${RED}❌ Không tìm thấy file benchmark nào.${RESET}"
        return
    fi
    
    echo -e "${BLUE}Các file có sẵn:${RESET}"
    for i in "${!files[@]}"; do
        echo -e "  $((i+1)). $(basename "${files[i]}")"
    done
    
    echo -n "Chọn số thứ tự (1-${#files[@]}): "
    read choice
    
    if [[ "$choice" =~ ^[0-9]+$ ]] && [ "$choice" -ge 1 ] && [ "$choice" -le "${#files[@]}" ]; then
        local selected_file="${files[$((choice-1))]}"
        echo -e "${GREEN}File: $(basename "$selected_file")${RESET}"
        echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${RESET}"
        cat "$selected_file"
    else
        echo -e "${RED}❌ Lựa chọn không hợp lệ.${RESET}"
    fi
}

clean_old() {
    echo -e "${CYAN}🧹 Dọn dẹp file cũ (giữ lại 5 file gần nhất):${RESET}"
    
    local files=($(ls $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | sort -V -r))
    local total=${#files[@]}
    
    if [ $total -le 5 ]; then
        echo -e "${GREEN}✅ Chỉ có $total file, không cần dọn dẹp.${RESET}"
        return
    fi
    
    echo -e "${YELLOW}📊 Tìm thấy $total file, sẽ xóa $((total-5)) file cũ nhất.${RESET}"
    
    for i in $(seq 5 $((total-1))); do
        local file_to_delete="${files[i]}"
        echo -e "  🗑️  Xóa: $(basename "$file_to_delete")"
        rm "$file_to_delete"
    done
    
    echo -e "${GREEN}✅ Dọn dẹp hoàn thành!${RESET}"
}

show_stats() {
    echo -e "${CYAN}📊 Thống kê benchmark:${RESET}"
    
    if [ ! -d "$BENCHMARK_DIR" ]; then
        echo -e "${RED}❌ Thư mục benchmark không tồn tại.${RESET}"
        return
    fi
    
    local total_files=$(ls -1 $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | wc -l)
    local total_size=$(du -sh $BENCHMARK_DIR 2>/dev/null | cut -f1)
    local latest_file=$(ls $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | sort -V | tail -1)
    local oldest_file=$(ls $BENCHMARK_DIR/ket_qua_benchmark-v*.txt 2>/dev/null | sort -V | head -1)
    
    echo -e "  📁 Tổng số file: ${GREEN}$total_files${RESET}"
    echo -e "  💾 Dung lượng: ${GREEN}$total_size${RESET}"
    
    if [ -n "$latest_file" ]; then
        echo -e "  📄 File mới nhất: ${GREEN}$(basename "$latest_file")${RESET}"
        echo -e "  📄 File cũ nhất: ${GREEN}$(basename "$oldest_file")${RESET}"
    fi
}

# Main logic
case "${1:-help}" in
    "list")
        list_benchmarks
        ;;
    "latest")
        view_latest
        ;;
    "view")
        view_specific
        ;;
    "clean")
        clean_old
        ;;
    "stats")
        show_stats
        ;;
    "help"|*)
        show_help
        ;;
esac 