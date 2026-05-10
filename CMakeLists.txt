cmake_minimum_required(VERSION 3.20)
project(scheduler VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(scheduler src/main.cpp)

target_include_directories(scheduler PRIVATE include)

target_compile_options(scheduler PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)

find_package(Threads REQUIRED)
target_link_libraries(scheduler PRIVATE Threads::Threads)
