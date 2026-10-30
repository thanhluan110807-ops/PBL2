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
    # Python chỉ phục vụ kiểm tra nguồn, không phải dependency của thư viện lõi.
    find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter)
    set(CongCuKiemTra "${CMAKE_CURRENT_SOURCE_DIR}/tools/check_project_sources.py")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CongCuKiemTra}")

    add_custom_target(check_project_sources
        COMMAND "${Python3_EXECUTABLE}" "${CongCuKiemTra}" --root "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Kiem tra stdexcept va phu thuoc Qt trong ma du an"
        VERBATIM)

    # Công cụ dùng cùng quy tắc phạm vi cho quét nguồn và chọn header lõi.
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${CongCuKiemTra}"
                --root "${CMAKE_CURRENT_SOURCE_DIR}" --list-core-headers
        RESULT_VARIABLE MaKetQua
        OUTPUT_VARIABLE DanhSachHeader
        ERROR_VARIABLE LoiDanhSachHeader
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT MaKetQua EQUAL 0)
        message(FATAL_ERROR "Khong the lay danh sach header loi: ${LoiDanhSachHeader}")
    endif()
    string(REPLACE "\n" ";" DanhSachHeader "${DanhSachHeader}")
    set(CacTargetHeader)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/header_checks")
    foreach(Header IN LISTS DanhSachHeader)
        string(REGEX REPLACE "^include/" "" TenInclude "${Header}")
        string(MAKE_C_IDENTIFIER "${Header}" TenTepHeader)
        string(MD5 MaHeader "${Header}")
        set(TenTarget "pbl2_header_${MaHeader}")
        set(TepKiemTra "${CMAKE_CURRENT_BINARY_DIR}/header_checks/${TenTepHeader}_${MaHeader}.cpp")
        file(WRITE "${TepKiemTra}" "#include \"${TenInclude}\"\n")
        # Mỗi target chỉ có một TU, không include trước header khác hoặc liên kết GUI.
        add_library(${TenTarget} OBJECT "${TepKiemTra}")
        target_include_directories(${TenTarget} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_link_libraries(${TenTarget} PRIVATE pbl2_build_options)
        list(APPEND CacTargetHeader ${TenTarget})
    endforeach()
    list(LENGTH DanhSachHeader SoHeader)
    message(STATUS "Independent core headers: ${SoHeader}")
    add_custom_target(check_core_headers DEPENDS ${CacTargetHeader})

    add_test(NAME check_project_sources
        COMMAND "${Python3_EXECUTABLE}" "${CongCuKiemTra}" --root "${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME test_project_checks
        COMMAND "${Python3_EXECUTABLE}" -m unittest discover
                -s "${CMAKE_CURRENT_SOURCE_DIR}/tests/checks" -p "test_*.py" -v)
    set_tests_properties(check_project_sources test_project_checks PROPERTIES TIMEOUT 60)

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

# Demo Qt tách biệt; cấu hình mặc định của lõi/CI không dò hay liên kết Qt.
option(PBL2_BUILD_QT_DEMO "Build demo dieu phoi cap cuu bang Qt Widgets" OFF)
if(PBL2_BUILD_QT_DEMO)
    add_subdirectory(demo/qt-triage)
endif()
