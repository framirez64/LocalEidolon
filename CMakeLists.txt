cmake_minimum_required(VERSION 3.16)
project(Eidolon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Add include directories if needed
include_directories(${CMAKE_SOURCE_DIR}/ui)
include_directories(
  N:/Users/GitHub/LocalEidolon/uWebSockets/src
  N:/Users/GitHub/LocalEidolon/uWebSockets/uSockets/src
)
add_executable(Eidolon
  ui/main.cpp
  ui/trayapp.cpp
  ui/trayapp.h
  ui/conversationwindow.cpp
  ui/conversationwindow.h
  ui/llmserver.cpp        # <-- Add this
  ui/llmserver.h          # <-- Add this
  resources.qrc
)

# Link Qt and uWebSockets dependencies
target_link_libraries(Eidolon
  Qt6::Widgets
  uWS
  ssl
  crypto
  z
  stdc++fs
  pthread
)

set_target_properties(Eidolon PROPERTIES WIN32_EXECUTABLE TRUE)
