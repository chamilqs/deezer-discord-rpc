cmake_minimum_required(VERSION 3.16)

project(DeezerDiscordRPC VERSION 1.3.7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineWidgets Network)

# Discord RPC library
# You can either use find_package or add as subdirectory
# Option 1: If discord-rpc is installed system-wide
# find_package(discord-rpc REQUIRED)

# Option 2: Add as git submodule (recommended)
# Execute: git submodule add https://github.com/discord/discord-rpc external/discord-rpc
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/discord-rpc/CMakeLists.txt")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/discord-rpc)
else()
    # If not using submodule, try to find it
    find_library(DISCORD_RPC_LIBRARY NAMES discord-rpc)
    find_path(DISCORD_RPC_INCLUDE_DIR discord_rpc.h)
    if(NOT DISCORD_RPC_LIBRARY OR NOT DISCORD_RPC_INCLUDE_DIR)
        message(WARNING "discord-rpc not found. Please either:")
        message("  1. Install discord-rpc system-wide, or")
        message("  2. Add it as a git submodule: git submodule add https://github.com/discord/discord-rpc cpp/external/discord-rpc")
        message("  3. Or manually download and place discord-rpc in cpp/external/discord-rpc")
    endif()
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/DeezerWindow.cpp
    src/DiscordRPC.cpp
    src/Config.cpp
    src/TrayIcon.cpp
)

set(HEADERS
    src/DeezerWindow.h
    src/DiscordRPC.h
    src/Config.h
    src/TrayIcon.h
)

set(RESOURCES
    resources.qrc
)

# Create executable
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
    set(APP_ICON_MACOSX "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app.icns")
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${RESOURCES} ${APP_ICON_MACOSX})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
)

# Link discord-rpc
if(TARGET discord-rpc)
    target_link_libraries(${PROJECT_NAME} PRIVATE discord-rpc)
elseif(DISCORD_RPC_LIBRARY)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DISCORD_RPC_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${DISCORD_RPC_LIBRARY})
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    # Add icon resource
    target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app.ico")
endif()

# macOS specific settings
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Deezer Discord RPC"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.deezer.discordrpc"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )
endif()

# Install targets
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Install Qt dependencies (for deployment)
if(WIN32)
    # Windows: use windeployqt
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Running windeployqt..."
        )
    endif()
elseif(APPLE)
    # macOS: use macdeployqt
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    
    if(MACDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${MACDEPLOYQT_EXECUTABLE}" "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
            COMMENT "Running macdeployqt..."
        )
    endif()
endif()

# CPack configuration for creating installers
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "DeezerDiscordRPC")
set(CPACK_PACKAGE_VENDOR "Deezer Discord RPC")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Discord Rich Presence for Deezer")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "DeezerDiscordRPC")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "Deezer Discord RPC")
    set(CPACK_NSIS_PACKAGE_NAME "Deezer Discord RPC")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Deezer Discord RPC")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6webenginewidgets6")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, qt6-qtwebengine")
endif()

include(CPack)
