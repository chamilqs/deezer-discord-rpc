@echo off
REM Build script for Deezer Discord RPC C++ implementation (Windows)

echo Building Deezer Discord RPC (C++ Implementation)

REM Check for CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: cmake is required but not installed.
    exit /b 1
)

REM Check for Qt
where qmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    where qmake6 >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo Error: Qt 6 is required but not installed.
        exit /b 1
    )
)

REM Get Qt path (you may need to adjust this)
REM Common Qt paths:
REM C:\Qt\6.5.0\msvc2019_64
REM C:\Qt\6.5.0\mingw_64

if defined QT_DIR (
    set QT_PATH=%QT_DIR%
) else (
    echo Warning: QT_DIR not set. Using default path.
    set QT_PATH=C:\Qt\6.5.0\msvc2019_64
)

echo Qt path: %QT_PATH%

REM Create build directory
if exist build (
    echo Build directory exists, removing...
    rmdir /s /q build
)

mkdir build
cd build

echo Running CMake configuration...

REM Configure with CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%"
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    exit /b 1
)

echo Building project...

REM Build
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo Build complete!
echo Executable: build\Release\DeezerDiscordRPC.exe

echo To run the application:
echo   build\Release\DeezerDiscordRPC.exe

pause
