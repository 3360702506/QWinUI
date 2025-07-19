# 简单的widget示例编译脚本
# 使用方法: 在QWinUI根目录下运行
# cmake -P examples/widget_demo.cmake

cmake_minimum_required(VERSION 3.16)

# 设置项目
project(QWinUIWidgetDemo)

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 启用MOC
set(CMAKE_AUTOMOC ON)

# 创建可执行文件
add_executable(widget_demo examples/widget.cpp)

# 链接库
target_link_libraries(widget_demo
    Qt6::Core
    Qt6::Widgets
    QWinUI
)

# 包含目录
target_include_directories(widget_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

message(STATUS "Widget demo configured successfully!")
message(STATUS "Build with: cmake --build . --target widget_demo")
