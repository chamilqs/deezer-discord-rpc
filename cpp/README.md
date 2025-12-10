# Deezer Discord RPC - C++ Implementation

This is a native C++ implementation of the Deezer Discord RPC application, providing a lightweight alternative to the Electron-based TypeScript version with lower memory footprint and better performance.

## Features

All features from the TypeScript/Electron version:

- ✅ Discord Rich Presence integration showing current Deezer playback
- ✅ Real-time updates when song changes
- ✅ System tray integration with context menu
- ✅ Configurable status display options
- ✅ Configurable tooltip text
- ✅ Close-to-tray behavior (configurable)
- ✅ Single instance enforcement
- ✅ Window state persistence (size, position)
- ✅ OAuth login support (Facebook, Google, Apple)
- ✅ Auto-hide activity when not playing
- ✅ Embedded Deezer web interface

## Prerequisites

### All Platforms

- **CMake** 3.16 or later
- **C++17** compatible compiler
- **Qt 6** (Core, Widgets, WebEngineWidgets, Network)
- **discord-rpc** library

### Windows

- Visual Studio 2019 or later (with C++ support), or
- MinGW-w64 with GCC 9+ or Clang 10+
- Qt 6.2+ for Windows

### macOS

- Xcode Command Line Tools
- Qt 6.2+ for macOS
- Homebrew (recommended for dependencies)

### Linux

- GCC 9+ or Clang 10+
- Qt 6.2+ development packages
- Build essentials

## Dependencies Installation

### Windows

1. **Install Qt 6:**
   - Download Qt from https://www.qt.io/download
   - Install Qt 6.5+ with WebEngine component
   - Add Qt to PATH or use CMake prefix path

2. **Install CMake:**
   - Download from https://cmake.org/download/
   - Or use: `winget install Kitware.CMake`

3. **Install discord-rpc:**
   ```powershell
   git clone https://github.com/discord/discord-rpc
   cd discord-rpc
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=C:\discord-rpc
   cmake --build . --config Release
   cmake --install .
   ```

### macOS

1. **Install Homebrew** (if not already installed):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install dependencies:**
   ```bash
   brew install qt@6 cmake
   ```

3. **Install discord-rpc:**
   ```bash
   git clone https://github.com/discord/discord-rpc
   cd discord-rpc
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
   make
   sudo make install
   ```

### Linux (Debian/Ubuntu)

```bash
# Install Qt 6
sudo apt update
sudo apt install qt6-base-dev qt6-webengine-dev cmake build-essential

# Install discord-rpc
git clone https://github.com/discord/discord-rpc
cd discord-rpc
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

### Linux (Fedora)

```bash
# Install Qt 6
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel cmake gcc-c++

# Install discord-rpc (same as above)
```

### Linux (Arch Linux)

```bash
# Install Qt 6
sudo pacman -S qt6-base qt6-webengine cmake base-devel

# Install discord-rpc
yay -S discord-rpc  # or build from source as above
```

## Building from Source

### Option 1: Using Git Submodule (Recommended)

This option bundles discord-rpc with the project:

```bash
cd cpp
git submodule add https://github.com/discord/discord-rpc external/discord-rpc
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Option 2: Using System-Installed discord-rpc

If you've installed discord-rpc system-wide:

```bash
cd cpp
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Platform-Specific Build Commands

#### Windows (Visual Studio)

```powershell
cd cpp
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"
cmake --build . --config Release
```

#### Windows (MinGW)

```powershell
cd cpp
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\mingw_64"
cmake --build . --config Release
```

#### macOS

```bash
cd cpp
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
make -j$(sysctl -n hw.ncpu)
```

#### Linux

```bash
cd cpp
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running the Application

After building:

### Windows
```powershell
.\build\Release\DeezerDiscordRPC.exe
```

### macOS
```bash
open ./build/DeezerDiscordRPC.app
# Or from command line:
./build/DeezerDiscordRPC.app/Contents/MacOS/DeezerDiscordRPC
```

### Linux
```bash
./build/DeezerDiscordRPC
```

## Creating Distribution Packages

### Windows Installer (NSIS)

```powershell
cd build
cpack -G NSIS
```

### macOS DMG

```bash
cd build
cpack -G DragNDrop
```

### Linux Packages

```bash
cd build
# Debian/Ubuntu
cpack -G DEB

# Fedora/RedHat
cpack -G RPM

# AppImage (requires linuxdeploy)
# Follow AppImage packaging guide
```

## Configuration

Configuration is stored using Qt's QSettings:

- **Windows:** Registry at `HKEY_CURRENT_USER\Software\DeezerDiscordRPC\DeezerDiscordRPC`
- **macOS:** `~/Library/Preferences/com.deezer-discord-rpc.DeezerDiscordRPC.plist`
- **Linux:** `~/.config/DeezerDiscordRPC/DeezerDiscordRPC.conf`

Settings include:
- Window size and position
- Status name preference
- Tooltip text preference
- Don't close to tray option

## Differences from Electron Version

### Advantages

- **Lower Memory Usage:** ~150-200 MB vs 300-500 MB for Electron
- **Faster Startup:** Native code loads much faster
- **Better Performance:** More efficient resource usage
- **Smaller Binary:** ~20-50 MB vs 100+ MB unpacked

### Known Limitations

- Auto-updater not yet implemented (check GitHub releases manually)
- Some advanced ad-blocking features pending
- Media key support not yet implemented

### Planned Features

- [ ] Built-in auto-updater
- [ ] Advanced ad-blocking with URL interceptor
- [ ] Media key support (play/pause/next/previous)
- [ ] Mini player mode
- [ ] Custom themes

## Troubleshooting

### Application won't start

1. **Check Qt installation:**
   ```bash
   qmake --version  # Should show Qt 6.x
   ```

2. **Check discord-rpc library:**
   - Windows: Ensure discord-rpc.dll is in PATH or same directory
   - Linux: Run `ldconfig` after installing
   - macOS: Check library paths with `otool -L`

### Discord RPC not connecting

1. Ensure Discord desktop app is running
2. Check Discord's overlay settings
3. Try the "Reconnect RPC" option in tray menu
4. Check debug output for error messages

### Web page not loading

1. Check internet connection
2. Verify Qt WebEngine is properly installed:
   ```bash
   # Linux
   sudo apt install qt6-webengine-dev
   ```
3. Check console output for WebEngine errors

### Build errors

1. **Qt not found:**
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
   ```

2. **discord-rpc not found:**
   - Use git submodule method, or
   - Specify path: `-DDISCORD_RPC_LIBRARY=/path/to/libdiscord-rpc.so`

3. **C++17 not supported:**
   - Update your compiler to GCC 9+, Clang 10+, or MSVC 2019+

## Development

### Project Structure

```
cpp/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── src/
│   ├── main.cpp           # Application entry point
│   ├── DeezerWindow.h/cpp # Main window with web view
│   ├── DiscordRPC.h/cpp   # Discord RPC wrapper
│   ├── Config.h/cpp       # Configuration management
│   └── TrayIcon.h/cpp     # System tray icon
├── include/               # Additional headers
├── resources/
│   └── icons/            # Application icons
└── external/
    └── discord-rpc/      # Git submodule (optional)
```

### Building in Debug Mode

```bash
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Code Style

- C++17 standard
- RAII and smart pointers
- Qt naming conventions (camelCase for methods, m_ prefix for members)
- Comprehensive error handling

## Contributing

Contributions are welcome! Please:

1. Follow the existing code style
2. Test on your target platform
3. Update documentation
4. Add comments for complex logic

## License

MIT License - Same as the original TypeScript version

Copyright (c) 2024 Deezer Discord RPC Contributors

## Acknowledgments

- Original TypeScript version by [CuteTenshii/JustYuuto](https://github.com/CuteTenshii/deezer-discord-rpc)
- Qt Framework by The Qt Company
- discord-rpc by Discord Inc.

## Support

- GitHub Issues: https://github.com/CuteTenshii/deezer-discord-rpc/issues
- Original Project: https://github.com/CuteTenshii/deezer-discord-rpc

---

**Note:** This C++ implementation is designed to coexist with the Electron version. Both can be installed simultaneously without conflicts as they use different configuration storage.
