cmake_minimum_required(VERSION 3.10)
project(SmartHospital CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # để VS Code (clangd) biết đường dẫn include/
add_compile_options(-Wall -Wextra)

include_directories(include)

# Các file cài đặt dùng chung (không gồm main)
set(CORE_SOURCES
    src/model/Person.cpp
    src/model/Patient.cpp
    src/repository/PatientFileManager.cpp
    src/util/my_string.cpp
)

add_executable(hospital_app src/main.cpp ${CORE_SOURCES})
add_executable(run_tests tests/test_triage_queue.cpp ${CORE_SOURCES})

# ---------------- Giao diện Qt (Qt Widgets) ----------------
# Nếu Qt cài ngoài hệ thống: cmake .. -DCMAKE_PREFIX_PATH=~/Qt/6.8.3/gcc_64
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets)
if(QT_FOUND)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    set(CMAKE_AUTOMOC ON)

    add_executable(hospital_gui
        src/ui/gui_main.cpp
        src/ui/MainWindow.cpp
        src/ui/AddEditPatientDialog.cpp
        include/ui/MainWindow.h
        include/ui/AddEditPatientDialog.h
        src/service/QueuePressureCalculator.cpp
        ${CORE_SOURCES})
    target_link_libraries(hospital_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
else()
    message(STATUS "Không tìm thấy Qt - bỏ qua target hospital_gui")
endif()
