cmake_minimum_required(VERSION 3.20)
project(PBL2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PBL2_ENABLE_SANITIZERS "Bat AddressSanitizer va UndefinedBehaviorSanitizer" OFF)

# Cấu hình dùng chung cho phần lõi và test; không phụ thuộc Qt.
add_library(pbl2_build_options INTERFACE)
target_compile_features(pbl2_build_options INTERFACE cxx_std_17)
if(MSVC)
    target_compile_options(pbl2_build_options INTERFACE /W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    target_compile_options(pbl2_build_options INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

if(PBL2_ENABLE_SANITIZERS)
    if(MSVC OR NOT CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        message(FATAL_ERROR "Sanitizer can compiler GCC/Clang ho tro AddressSanitizer va UndefinedBehaviorSanitizer")
    endif()
    target_compile_options(pbl2_build_options INTERFACE
        -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(pbl2_build_options INTERFACE -fsanitize=address,undefined)
endif()

# GUI trong tương lai có thể liên kết phần lõi này, không chứa main hoặc mã hiển thị.
add_library(pbl2_core STATIC src/model/Person.cpp)
target_include_directories(pbl2_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(pbl2_core PUBLIC pbl2_build_options)

# BUILD_TESTING mặc định ON; OFF cho phép chỉ biên dịch thư viện lõi.
include(CTest)
if(BUILD_TESTING)
    add_executable(test_person tests/model/test_person.cpp)
    target_link_libraries(test_person PRIVATE pbl2_core)

    # Ngoại lệ và DSA là header-only, không cần liên kết với model Person.
    add_executable(test_my_stdexcept tests/util/test_my_stdexcept.cpp)
    target_include_directories(test_my_stdexcept PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(test_my_stdexcept PRIVATE pbl2_build_options)

    foreach(TenKiemThu IN ITEMS test_person test_my_stdexcept)
        # Release thường đặt NDEBUG; bỏ macro này để assert vẫn thực sự được chạy.
        if(MSVC)
            target_compile_options(${TenKiemThu} PRIVATE /UNDEBUG)
        else()
            target_compile_options(${TenKiemThu} PRIVATE -UNDEBUG)
        endif()
        add_test(NAME ${TenKiemThu} COMMAND ${TenKiemThu})
        set_tests_properties(${TenKiemThu} PROPERTIES TIMEOUT 30)
        if(PBL2_ENABLE_SANITIZERS)
            # UBSan phải trả lỗi ngay, không chỉ in cảnh báo rồi cho test tiếp tục.
            set_tests_properties(${TenKiemThu} PROPERTIES
                ENVIRONMENT "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1")
        endif()
    endforeach()
endif()
