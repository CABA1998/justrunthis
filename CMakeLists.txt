cmake_minimum_required(VERSION 3.16)
project(justrunthis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(justrunthis WIN32
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/CppRunner.cpp
    src/CppRunner.h
)

target_link_libraries(justrunthis PRIVATE Qt6::Widgets)
