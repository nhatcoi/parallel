#include "../../include/sort_ogt.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for isatty()

// Comprehensive testing menu - main interface for the library
void overallTestOGT() {
    int choice;
    
    // Show library info at startup
    printLibraryInfo();
    
    do {
        printf("\n" BLUE "=== SORT OGT LIBRARY - MAIN TESTING MENU ===" RESET "\n");
        printf("1. 🔤 Demo Cơ Bản (Basic Sorting Demo)\n");
        printf("2. ⚡ Demo Song Song Nhanh (Quick Parallel Demo)\n");
        printf("3. 🧪 Test Tuần Tự - Nhập Mảng (Sequential User Input)\n");
        printf("4. 🎲 Test Song Song - Mảng Ngẫu Nhiên (Parallel Random Array)\n");
        printf("5. 📊 So Sánh Performance Threads (Thread Performance)\n");
        printf("6. 🔬 Menu Test Tương Tác Đầy Đủ (Full Interactive Tests)\n");
        printf("7. 📈 Benchmark Đơn (Single Array Size)\n");
        printf("8. 📊 Benchmark Đầy Đủ (Complete Benchmark Suite)\n");
        printf("9. ✅ Kiểm Tra Tính Đúng (Correctness Verification)\n");
        printf("10. ℹ️  Thông Tin Hệ Thống (System Information)\n");
        printf("11. 📋 Thông Tin Thư Viện (Library Information)\n");
        printf("0. 🚪 Thoát (Exit)\n");
        printf("Lựa chọn: ");
        
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("\n" GREEN "=== CASE 1: DEMO CƠ BẢN ===" RESET "\n");
                runBasicSortingDemo();
                break;
                
            case 2:
                printf("\n" CYAN "=== CASE 2: DEMO SONG SONG NHANH ===" RESET "\n");
                runQuickParallelDemo();
                break;
                
            case 3:
                printf("\n" YELLOW "=== CASE 3: TEST TUẦN TỰ - NHẬP MẢNG ===" RESET "\n");
                testSequentialUserInput();
                break;
                
            case 4:
                printf("\n" MAGENTA "=== CASE 4: TEST SONG SONG - MẢNG NGẪU NHIÊN ===" RESET "\n");
                testParallelRandomArray();
                break;
                
            case 5:
                printf("\n" BLUE "=== CASE 5: SO SÁNH PERFORMANCE THREADS ===" RESET "\n");
                testThreadPerformanceComparison();
                break;
                
            case 6:
                printf("\n" GREEN "=== CASE 6: MENU TEST TƯƠNG TÁC ĐẦY ĐỦ ===" RESET "\n");
                runInteractiveTests();
                break;
                
            case 7: {
                printf("\n" CYAN "=== CASE 7: BENCHMARK ĐơN ===" RESET "\n");
                int array_size;
                printf("Nhập kích thước mảng để benchmark (1000-100000): ");
                scanf("%d", &array_size);
                
                if (array_size >= 1000 && array_size <= 100000) {
                    runSingleBenchmark(array_size);
                } else {
                    printf(RED "❌ Kích thước không hợp lệ! (1000-100000)" RESET "\n");
                }
                break;
            }
            
            case 8:
                printf("\n" YELLOW "=== CASE 8: BENCHMARK ĐẦY ĐỦ ===" RESET "\n");
                runCompleteBenchmark();
                break;
                
            case 9:
                printf("\n" GREEN "=== CASE 9: KIỂM TRA TÍNH ĐÚNG ===" RESET "\n");
                demonstrateSortingCorrectness();
                break;
                
            case 10:
                printf("\n" BLUE "=== CASE 10: THÔNG TIN HỆ THỐNG ===" RESET "\n");
                printSystemInformation();
                break;
                
            case 11:
                printf("\n" MAGENTA "=== CASE 11: THÔNG TIN THƯ VIỆN ===" RESET "\n");
                printLibraryInfo();
                printf("\n" CYAN "Available Functions:" RESET "\n");
                printf("• insertionSortAsc() - Sort tuần tự tăng dần\n");
                printf("• insertionSortDesc() - Sort tuần tự giảm dần\n");
                printf("• parallelInsertionSortAsc() - Sort song song tăng dần\n");
                printf("• parallelInsertionSortDesc() - Sort song song giảm dần\n");
                printf("• getCurrentTime() - Lấy thời gian hiện tại\n");
                printf("• copyArray() - Copy mảng\n");
                printf("• generateRandomArray() - Tạo mảng ngẫu nhiên\n");
                printf("• runCompleteBenchmark() - Chạy benchmark đầy đủ\n");
                printf("• runInteractiveTests() - Menu test tương tác\n");
                printf("• overallTestOGT() - Menu test tổng thể\n");
                break;
                
            case 0:
                printf(GREEN "\n👋 Goodbye from Sort OGT Library!\n");
                printf("Thank you for testing!" RESET "\n");
                break;
                
            default:
                printf(RED "❌ Lựa chọn không hợp lệ! Vui lòng chọn 0-11." RESET "\n");
                break;
        }
        
        // Press enter to continue (except for exit)
        if (choice != 0) {
            printf("\n" YELLOW "Nhấn Enter để tiếp tục..." RESET);
            
            // Clear input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            // Wait for enter only in interactive mode
            if (isatty(0)) {  // Check if input is from terminal
                getchar();
            } else {
                printf("(Auto-continuing in non-interactive mode)\n");
            }
        }
        
    } while (choice != 0);
} 