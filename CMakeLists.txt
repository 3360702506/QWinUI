cmake_minimum_required(VERSION 3.16)

project(QWinUIDemo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6（QWinUI需要Qt6）
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 添加QWinUI子项目
add_subdirectory(QWinUI)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        widget.h
)

# 创建主应用程序可执行文件
qt_add_executable(QWinUIDemo
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# 链接Qt库和QWinUI库
target_link_libraries(QWinUIDemo PRIVATE
    Qt6::Core
    Qt6::Widgets
    QWinUI
)

# 包含QWinUI头文件
target_include_directories(QWinUIDemo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/QWinUI/include
)

# 设置目标属性
set_target_properties(QWinUIDemo PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QWinUIDemo
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Windows特定设置
if(WIN32)
    target_link_libraries(QWinUIDemo PRIVATE dwmapi)
endif()

include(GNUInstallDirs)
install(TARGETS QWinUIDemo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(QWinUIDemo)
