# Widget Demo CMake 配置
# 在QWinUI根目录下使用以下命令编译:
# mkdir widget_build
# cd widget_build
# cmake -f ../widget_demo.cmake ..
# cmake --build .

cmake_minimum_required(VERSION 3.16)

project(QWinUIWidgetDemo VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 启用Qt的MOC
set(CMAKE_AUTOMOC ON)

# 包含QWinUI库的目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 链接QWinUI库目录
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/lib)

# 创建widget demo可执行文件
add_executable(widget_demo widget.cpp)

# 链接库
target_link_libraries(widget_demo
    Qt6::Core
    Qt6::Widgets
    QWinUI
)

# Windows特定设置
if(WIN32)
    target_link_libraries(widget_demo dwmapi user32 version)
endif()

message(STATUS "Widget demo configured!")
message(STATUS "Build with: cmake --build . --target widget_demo")
