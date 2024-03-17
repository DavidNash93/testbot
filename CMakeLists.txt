cmake_minimum_required(VERSION 3.10)
project(botty)

set(CMAKE_C_STANDARD 99)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(botty ${SOURCES})

if(WIN32)
  target_link_libraries(botty ws2_32)
endif()