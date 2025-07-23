cmake_minimum_required(VERSION 3.16)

project(QWinUI VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 查找Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg SvgWidgets)

# 启用Qt的MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 源文件
set(QWINUI_SOURCES
    src/QWinUIWidget.cpp
    src/QWinUITheme.cpp
    src/QWinUIIconManager.cpp
    src/QWinUIAnimation.cpp
    src/QWinUIBlurEffect.cpp
    src/QWinUI.cpp
    src/Controls/QWinUITextBlock.cpp
    src/Controls/QWinUIRichTextBlock.cpp
    src/Controls/QWinUIButton.cpp
    src/Controls/QWinUIIconButton.cpp
    src/Controls/QWinUIToggleButton.cpp
    src/Controls/QWinUIToggleSwitch.cpp
    src/Controls/QWinUIToolTip.cpp
    src/Controls/QWinUIProgressBar.cpp
    src/Controls/QWinUIProgressRing.cpp
    src/Controls/QWinUIIcon.cpp
    src/Controls/QWinUIAcrylicBrush.cpp
    src/Controls/QWinUIRichEditBox.cpp
    src/Controls/QWinUITextInput.cpp
    src/Controls/QWinUITextBox.cpp
    src/Controls/QWinUIPasswordBox.cpp
    src/Controls/QWinUIScrollView.cpp
    src/Controls/QWinUIScrollBar.cpp
    src/Controls/QWinUINotification.cpp
    src/Controls/QWinUIAppBarSeparator.cpp
    src/Controls/QWinUIVariableSizedWrapGrid.cpp
    src/Controls/QWinUIMenuFlyout.cpp
    src/Controls/QWinUISplitButton.cpp
    src/Controls/QWinUIRadioButton.cpp
    src/Controls/QWinUISlider.cpp
    src/Layouts/QWinUIFlowLayout.cpp
)

# 头文件
set(QWINUI_HEADERS
    include/QWinUI/QWinUIGlobal.h
    include/QWinUI/QWinUIWidget.h
    include/QWinUI/QWinUITheme.h
    include/QWinUI/QWinUIIconManager.h
    include/QWinUI/QWinUIIconConstants.h
    include/QWinUI/QWinUIFluentIcons.h
    include/QWinUI/QWinUIAnimation.h
    include/QWinUI/QWinUIBlurEffect.h
    include/QWinUI/QWinUI.h
    include/QWinUI/Controls/QWinUITextBlock.h
    include/QWinUI/Controls/QWinUIRichTextBlock.h
    include/QWinUI/Controls/QWinUIButton.h
    include/QWinUI/Controls/QWinUIIconButton.h
    include/QWinUI/Controls/QWinUIToggleButton.h
    include/QWinUI/Controls/QWinUIToggleSwitch.h
    include/QWinUI/Controls/QWinUIToolTip.h
    include/QWinUI/Controls/QWinUIProgressBar.h
    include/QWinUI/Controls/QWinUIProgressRing.h
    include/QWinUI/Controls/QWinUIIcon.h
    include/QWinUI/Controls/QWinUIAcrylicBrush.h
    include/QWinUI/Controls/QWinUIRichEditBox.h
    include/QWinUI/Controls/QWinUITextInput.h
    include/QWinUI/Controls/QWinUITextBox.h
    include/QWinUI/Controls/QWinUIPasswordBox.h
    include/QWinUI/Controls/QWinUIScrollView.h
    include/QWinUI/Controls/QWinUIScrollBar.h
    include/QWinUI/Controls/QWinUINotification.h
    include/QWinUI/Controls/QWinUIAppBarSeparator.h
    include/QWinUI/Controls/QWinUIVariableSizedWrapGrid.h
    include/QWinUI/Controls/QWinUIMenuFlyout.h
    include/QWinUI/Controls/QWinUISplitButton.h
    include/QWinUI/Controls/QWinUIRadioButton.h
    include/QWinUI/Controls/QWinUISlider.h
    include/QWinUI/Layouts/QWinUIFlowLayout.h
)

# 资源文件
set(QWINUI_RESOURCES
    resources/icons.qrc
)

# 创建库
add_library(QWinUI SHARED ${QWINUI_SOURCES} ${QWINUI_HEADERS} ${QWINUI_RESOURCES})

# 设置库的属性
set_target_properties(QWinUI PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${QWINUI_HEADERS}"
)

# 链接Qt库
target_link_libraries(QWinUI
    Qt6::Core
    Qt6::Widgets
    Qt6::Svg
    Qt6::SvgWidgets
)

# Windows特定设置
if(WIN32)
    target_link_libraries(QWinUI
        dwmapi
        user32
        version
        windowsapp  # Windows Runtime库
        runtimeobject  # Windows Runtime对象库
    )
    target_compile_definitions(QWinUI PRIVATE QT_BUILD_QWINUI_LIB)
endif()

# 编译器特定设置
if(MSVC)
    target_compile_options(QWinUI PRIVATE /W4)
else()
    target_compile_options(QWinUI PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 包含目录
target_include_directories(QWinUI
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 安装规则
include(GNUInstallDirs)

install(TARGETS QWinUI
    EXPORT QWinUITargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/QWinUI
)

# 安装头文件
install(DIRECTORY include/QWinUI
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# 创建和安装CMake配置文件
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    QWinUIConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QWinUIConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QWinUIConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QWinUI
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/QWinUIConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/QWinUIConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QWinUI
)

install(EXPORT QWinUITargets
    FILE QWinUITargets.cmake
    NAMESPACE QWinUI::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QWinUI
)

# 创建pkg-config文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/qwinui.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/qwinui.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qwinui.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# 选项：构建示例
option(BUILD_EXAMPLES "Build QWinUI examples" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Widget Demo 可执行文件
option(BUILD_WIDGET_DEMO "Build widget demo" OFF)
if(BUILD_WIDGET_DEMO)
    add_executable(widget_demo widget.cpp)
    target_link_libraries(widget_demo
        Qt6::Core
        Qt6::Widgets
        QWinUI
    )
    target_include_directories(widget_demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    if(WIN32)
        target_link_libraries(widget_demo dwmapi user32 version)
    endif()
endif()

# 选项：构建测试
option(BUILD_TESTS "Build QWinUI tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 选项：构建文档
option(BUILD_DOCS "Build QWinUI documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    else()
        message(WARNING "Doxygen not found, documentation will not be built")
    endif()
endif()

# 打印构建信息
message(STATUS "QWinUI ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
# message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build docs: ${BUILD_DOCS}")

# CPack配置（用于创建安装包）
set(CPACK_PACKAGE_NAME "QWinUI")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt WinUI3-style component library")
set(CPACK_PACKAGE_VENDOR "QWinUI Project")
set(CPACK_PACKAGE_CONTACT "qwinui@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "QWinUI ${PROJECT_VERSION}")
    set(CPACK_NSIS_PACKAGE_NAME "QWinUI")
    set(CPACK_NSIS_CONTACT "qwinui@example.com")
    set(CPACK_NSIS_HELP_LINK "https://github.com/qwinui/qwinui")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/qwinui/qwinui")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "QWinUI Project")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase")
endif()

include(CPack)
