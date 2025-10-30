cmake_minimum_required(VERSION 3.10)
project(chat_server VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prevent in-source build
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source build is not allowed. Create a separate build directory:\n  mkdir build && cd build && cmake ..")
endif()

# Optional inputs from user
set(MUDUO_ROOT "" CACHE PATH "Parent directory that contains the 'muduo' source tree or installed headers (e.g. /root/muduo)")
set(MUDUO_LIB_DIR "" CACHE PATH "Directory that contains muduo libraries (optional).")

# Candidate include search dirs
if (MUDUO_ROOT)
  set(_hints ${MUDUO_ROOT})
else()
  set(_hints /usr/local/include /usr/include)
endif()

find_path(MUDUO_INCLUDE_DIR
  NAMES muduo/net/TcpServer.h
  HINTS ${_hints}
)

# Candidate library search paths
set(_lib_paths "")
if (MUDUO_LIB_DIR)
  list(APPEND _lib_paths ${MUDUO_LIB_DIR})
endif()
list(APPEND _lib_paths /usr/local/lib /usr/lib /lib)

find_library(MUDUO_NET_LIBRARY NAMES muduo_net PATHS ${_lib_paths})
find_library(MUDUO_BASE_LIBRARY NAMES muduo_base PATHS ${_lib_paths})

message(STATUS "MUDUO_ROOT = ${MUDUO_ROOT}")
message(STATUS "MUDUO_INCLUDE_DIR = ${MUDUO_INCLUDE_DIR}")
message(STATUS "MUDUO_NET_LIBRARY = ${MUDUO_NET_LIBRARY}")
message(STATUS "MUDUO_BASE_LIBRARY = ${MUDUO_BASE_LIBRARY}")

if (NOT MUDUO_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find muduo headers (muduo/net/TcpServer.h).\nEither install muduo (sudo make install) or set -DMUDUO_ROOT=/path/to/muduo_parent_dir")
endif()

add_executable(chat_server main.cpp)

target_include_directories(chat_server PRIVATE ${MUDUO_INCLUDE_DIR})

# thread support
find_package(Threads REQUIRED)
target_link_libraries(chat_server PRIVATE Threads::Threads)

# link muduo libraries (require both)
if (MUDUO_BASE_LIBRARY AND MUDUO_NET_LIBRARY)
  target_link_libraries(chat_server PRIVATE ${MUDUO_BASE_LIBRARY} ${MUDUO_NET_LIBRARY})
else()
  message(FATAL_ERROR "Could not find muduo libraries (muduo_base / muduo_net). If you built muduo, set -DMUDUO_LIB_DIR to the directory containing libmuduo_base.a/.so and libmuduo_net.a/.so, or run 'sudo make install' in muduo.")
endif()

install(TARGETS chat_server RUNTIME DESTINATION bin)
