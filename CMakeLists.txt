cmake_minimum_required(VERSION 2.8)

# Options. Turn on with 'cmake -Dtest=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.

# Make PROJECT_SOURCE_DIR, PROJECT_BINARY_DIR, and PROJECT_NAME available.
set(PROJECT_NAME inverted_index)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-g -Wall -fPIC")

# So we can do include_directories(${COMMON_INCLUDES}) in other CMakeLists.txt files.
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)

# Include project directories
include_directories(${COMMON_INCLUDES})
include_directories(${IRCP_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/lib/utf8cpp/src)

add_subdirectory(src)

# Unit tests
include(CTest)

if (test)
  add_subdirectory(${CMAKE_SOURCE_DIR}/lib/gtest-1.7.0)
  add_subdirectory(test)
endif()
