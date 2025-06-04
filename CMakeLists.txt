cmake_minimum_required(VERSION 3.15)
project(Eidolon LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# Include app headers
include_directories(
    ${CMAKE_SOURCE_DIR}/ui
)

# Define preprocessor macros
add_compile_definitions(
    USE_WEBSOCKET  # Enable WebSocket support in CivetWeb
    NO_SSL         # Disable SSL (optional if not using HTTPS)
)

# Source files
set(CIVETWEB_SOURCES
    ui/civetweb.c
    ui/CivetServer.cpp
)

set(APP_SOURCES
    ui/main.cpp
    ui/trayapp.cpp
    ui/conversationwindow.cpp
    ui/llmserver.cpp
    ui/settingsdialog.cpp
    resources.qrc
)

# Executable
add_executable(Eidolon WIN32
    ${APP_SOURCES}
    ${CIVETWEB_SOURCES}
    ${RESOURCE_FILES}
)

# Ensure C source is treated as C
set_source_files_properties(ui/civetweb.c PROPERTIES LANGUAGE C)

# Link Qt libraries
target_link_libraries(Eidolon PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network)

# Link Winsock for Windows networking
if (WIN32)
    target_link_libraries(Eidolon PRIVATE ws2_32)
endif()
