cmake_minimum_required(VERSION 3.16)

project(QWinUI_BasicExample)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(QWinUI_BasicExample
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(QWinUI_BasicExample
    Qt6::Core
    Qt6::Widgets
    QWinUI
)

target_include_directories(QWinUI_BasicExample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
