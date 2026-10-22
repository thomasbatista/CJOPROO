cmake_minimum_required(VERSION 3.16)

project(FinanQt
    VERSION 1.0.0
    DESCRIPTION "Gestor de Despesas Pessoais em C++ e Qt 6"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(FinanQt
    src/main.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
)

target_include_directories(FinanQt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(FinanQt PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
