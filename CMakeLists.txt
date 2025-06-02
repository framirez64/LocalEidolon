cmake_minimum_required(VERSION 3.16)
project(Eidolon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(Eidolon
  ui/main.cpp
  ui/trayapp.cpp
  ui/trayapp.h
  ui/conversationwindow.cpp
  ui/conversationwindow.h
  resources.qrc
)

target_link_libraries(Eidolon Qt6::Widgets)
set_target_properties(Eidolon PROPERTIES WIN32_EXECUTABLE TRUE)