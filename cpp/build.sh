#!/bin/bash
# Build script for Deezer Discord RPC C++ implementation

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building Deezer Discord RPC (C++ Implementation)${NC}"

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="Windows"
else
    PLATFORM="Unknown"
fi

echo -e "Platform: ${YELLOW}$PLATFORM${NC}"

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo -e "${RED}Error: cmake is required but not installed.${NC}" >&2; exit 1; }
command -v qmake >/dev/null 2>&1 || command -v qmake6 >/dev/null 2>&1 || { echo -e "${RED}Error: Qt 6 (qmake/qmake6) is required but not installed.${NC}" >&2; exit 1; }

# Get Qt path
if command -v qmake6 >/dev/null 2>&1; then
    QT_PATH=$(qmake6 -query QT_INSTALL_PREFIX)
elif command -v qmake >/dev/null 2>&1; then
    QT_PATH=$(qmake -query QT_INSTALL_PREFIX)
fi

echo -e "Qt path: ${YELLOW}$QT_PATH${NC}"

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Build directory exists, removing...${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "${GREEN}Running CMake configuration...${NC}"

# Configure with CMake
if [ "$PLATFORM" == "macOS" ]; then
    # macOS specific
    cmake .. -DCMAKE_PREFIX_PATH="$QT_PATH" -DCMAKE_BUILD_TYPE=Release
elif [ "$PLATFORM" == "Linux" ]; then
    # Linux specific
    cmake .. -DCMAKE_PREFIX_PATH="$QT_PATH" -DCMAKE_BUILD_TYPE=Release
else
    # Generic
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

echo -e "${GREEN}Building project...${NC}"

# Build
if [ "$PLATFORM" == "Windows" ]; then
    cmake --build . --config Release
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)
fi

echo -e "${GREEN}Build complete!${NC}"

# Show output location
if [ "$PLATFORM" == "macOS" ]; then
    echo -e "Executable: ${YELLOW}$BUILD_DIR/DeezerDiscordRPC.app${NC}"
elif [ "$PLATFORM" == "Windows" ]; then
    echo -e "Executable: ${YELLOW}$BUILD_DIR/Release/DeezerDiscordRPC.exe${NC}"
else
    echo -e "Executable: ${YELLOW}$BUILD_DIR/DeezerDiscordRPC${NC}"
fi

echo -e "${GREEN}To run the application:${NC}"
if [ "$PLATFORM" == "macOS" ]; then
    echo -e "  ${YELLOW}open $BUILD_DIR/DeezerDiscordRPC.app${NC}"
elif [ "$PLATFORM" == "Windows" ]; then
    echo -e "  ${YELLOW}$BUILD_DIR/Release/DeezerDiscordRPC.exe${NC}"
else
    echo -e "  ${YELLOW}./$BUILD_DIR/DeezerDiscordRPC${NC}"
fi
