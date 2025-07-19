@echo off
echo Building QWinUI...

REM ??????
if not exist build mkdir build
cd build

REM ??CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release

REM ????
cmake --build . --config Release

echo Build completed!
pause
