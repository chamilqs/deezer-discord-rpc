# Building Deezer Discord RPC C++ Implementation

This document provides detailed instructions for building the C++ implementation on all supported platforms.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Detailed Build Instructions](#detailed-build-instructions)
  - [Linux](#linux)
  - [macOS](#macos)
  - [Windows](#windows)
- [Troubleshooting](#troubleshooting)
- [Advanced Configuration](#advanced-configuration)

## Prerequisites

### All Platforms

- **CMake** 3.16 or later
- **C++17** compatible compiler
- **Qt 6.2+** with WebEngine module
- **discord-rpc** library

## Quick Start

### Linux/macOS

```bash
cd cpp
chmod +x build.sh
./build.sh
```

### Windows

```batch
cd cpp
build.bat
```

## Detailed Build Instructions

### Linux

#### Ubuntu/Debian

1. **Install dependencies:**

```bash
# Update package list
sudo apt update

# Install Qt 6 and build tools
sudo apt install -y \
    build-essential \
    cmake \
    git \
    qt6-base-dev \
    qt6-webengine-dev \
    libgl1-mesa-dev

# Install discord-rpc
git clone https://github.com/discord/discord-rpc
cd discord-rpc
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
cd ../..
```

2. **Build the project:**

```bash
cd cpp
mkdir build && cd build
cmake ..
make -j$(nproc)
```

3. **Run:**

```bash
./DeezerDiscordRPC
```

#### Fedora

1. **Install dependencies:**

```bash
# Install Qt 6 and build tools
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    qt6-qtbase-devel \
    qt6-qtwebengine-devel \
    mesa-libGL-devel

# Install discord-rpc (same as Ubuntu)
```

2. **Build and run** (same as Ubuntu)

#### Arch Linux

1. **Install dependencies:**

```bash
# Install Qt 6 and build tools
sudo pacman -S --needed \
    base-devel \
    cmake \
    git \
    qt6-base \
    qt6-webengine

# Install discord-rpc from AUR
yay -S discord-rpc
# Or build from source (same as Ubuntu)
```

2. **Build and run** (same as Ubuntu)

### macOS

1. **Install Xcode Command Line Tools:**

```bash
xcode-select --install
```

2. **Install Homebrew** (if not already installed):

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

3. **Install dependencies:**

```bash
# Install Qt 6 and CMake
brew install qt@6 cmake

# Install discord-rpc
git clone https://github.com/discord/discord-rpc
cd discord-rpc
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(sysctl -n hw.ncpu)
sudo make install
cd ../..
```

4. **Build the project:**

```bash
cd cpp
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
make -j$(sysctl -n hw.ncpu)
```

5. **Run:**

```bash
open DeezerDiscordRPC.app
# Or from command line:
./DeezerDiscordRPC.app/Contents/MacOS/DeezerDiscordRPC
```

### Windows

#### Visual Studio 2019/2022

1. **Install Visual Studio:**
   - Download from https://visualstudio.microsoft.com/
   - Install with "Desktop development with C++" workload

2. **Install Qt 6:**
   - Download from https://www.qt.io/download
   - Install Qt 6.5+ with WebEngine component
   - Note the installation path (e.g., `C:\Qt\6.5.0\msvc2019_64`)

3. **Install CMake:**
   - Download from https://cmake.org/download/
   - Or use: `winget install Kitware.CMake`

4. **Install discord-rpc:**

Open PowerShell or Command Prompt:

```powershell
git clone https://github.com/discord/discord-rpc
cd discord-rpc
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX=C:\discord-rpc
cmake --build . --config Release
cmake --install .
cd ..\..
```

5. **Build the project:**

Option A: Using the build script:
```batch
cd cpp
build.bat
```

Option B: Manually:
```powershell
cd cpp
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"
cmake --build . --config Release
```

6. **Run:**

```batch
build\Release\DeezerDiscordRPC.exe
```

#### MinGW

1. **Install MinGW-w64:**
   - Download from https://www.mingw-w64.org/
   - Or use MSYS2: https://www.msys2.org/

2. **Install Qt 6 for MinGW:**
   - Download Qt for MinGW from https://www.qt.io/download
   - Note the installation path (e.g., `C:\Qt\6.5.0\mingw_64`)

3. **Install CMake and discord-rpc** (same as Visual Studio)

4. **Build the project:**

```batch
cd cpp
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\mingw_64"
cmake --build . --config Release
```

## Troubleshooting

### CMake cannot find Qt

**Solution:** Specify Qt path explicitly:

```bash
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
```

Example paths:
- Linux: `/usr/lib/x86_64-linux-gnu/cmake/Qt6`
- macOS: `/usr/local/opt/qt@6` or `$(brew --prefix qt@6)`
- Windows: `C:\Qt\6.5.0\msvc2019_64` or `C:\Qt\6.5.0\mingw_64`

### discord-rpc not found

**Option 1:** Use git submodule (recommended):

```bash
cd cpp
git submodule add https://github.com/discord/discord-rpc external/discord-rpc
cd external/discord-rpc
git checkout master
cd ../..
# Now rebuild
```

**Option 2:** Specify library path:

```bash
cmake .. -DDISCORD_RPC_LIBRARY=/path/to/libdiscord-rpc.so \
         -DDISCORD_RPC_INCLUDE_DIR=/path/to/discord-rpc/include
```

### Qt WebEngine not found

**Linux:**
```bash
sudo apt install qt6-webengine-dev
# or
sudo dnf install qt6-qtwebengine-devel
```

**macOS:**
```bash
brew reinstall qt@6
```

**Windows:**
Make sure to install the WebEngine component when installing Qt.

### Build errors with C++17

Update your compiler:
- GCC: Version 9 or later
- Clang: Version 10 or later  
- MSVC: Visual Studio 2019 or later

### Runtime error: "cannot find discord-rpc.dll" (Windows)

Copy `discord-rpc.dll` to the same directory as `DeezerDiscordRPC.exe`.

### Runtime error: Qt libraries not found

**Windows:**
Run `windeployqt`:
```batch
windeployqt build\Release\DeezerDiscordRPC.exe
```

**Linux:**
```bash
export LD_LIBRARY_PATH=/path/to/qt6/lib:$LD_LIBRARY_PATH
```

**macOS:**
Run `macdeployqt`:
```bash
macdeployqt build/DeezerDiscordRPC.app
```

## Advanced Configuration

### Static Linking

To reduce dependencies, enable static linking:

```bash
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6 \
         -DBUILD_SHARED_LIBS=OFF \
         -DQt6_DIR=/path/to/qt6/lib/cmake/Qt6
```

### Debug Build

```bash
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Custom Install Prefix

```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/custom/install/path
cmake --build .
cmake --install .
```

### Cross-Compilation

For cross-compiling (e.g., building Windows on Linux), see Qt documentation on cross-compilation:
https://doc.qt.io/qt-6/linux-building.html

### Creating Distribution Packages

#### AppImage (Linux)

```bash
# After building
cd build
# Download linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

# Create AppImage
./linuxdeploy-x86_64.AppImage --appdir AppDir -e DeezerDiscordRPC -i ../resources/icons/app.png -d ../resources/deezer-discord-rpc.desktop --plugin qt --output appimage
```

#### DMG (macOS)

```bash
cd build
cpack -G DragNDrop
```

#### NSIS Installer (Windows)

```batch
cd build
cpack -G NSIS
```

## Performance Tips

1. **Use Release build** for best performance:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

2. **Enable LTO (Link-Time Optimization)**:
   ```bash
   cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
   ```

3. **Parallel builds**:
   ```bash
   make -j$(nproc)  # Linux
   make -j$(sysctl -n hw.ncpu)  # macOS
   cmake --build . --parallel  # Windows
   ```

## Getting Help

If you encounter issues:

1. Check the [main README](README.md)
2. Search existing [GitHub issues](https://github.com/CuteTenshii/deezer-discord-rpc/issues)
3. Create a new issue with:
   - Your platform and version
   - CMake version (`cmake --version`)
   - Qt version (`qmake -version` or `qmake6 -version`)
   - Complete error output
   - Steps to reproduce

## Contributing

When submitting build-related PRs:
- Test on at least one platform
- Update this document if adding new dependencies
- Ensure CMakeLists.txt works across platforms
- Add comments for platform-specific code
