#!/bin/bash
echo "Building QWinUI..."

# 创建构建目录
mkdir -p build
cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 构建项目
cmake --build . --config Release

echo "Build completed!"
