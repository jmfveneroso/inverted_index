cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...

# Options. Turn on with 'cmake -Dtest=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.

# Make PROJECT_SOURCE_DIR, PROJECT_BINARY_DIR, and PROJECT_NAME available.
set(PROJECT_NAME inverted_index)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -pedantic -fPIC -O0 -D_FILE_OFFSET_BITS=64")
# set(CMAKE_CXX_FLAGS "-g -Wall -Werror -pedantic -fPIC -O3 -D_FILE_OFFSET_BITS=64")

# So we can do include_directories(${COMMON_INCLUDES}) in other CMakeLists.txt files.
set(COMMON_INCLUDES 
 ${PROJECT_SOURCE_DIR}/include 
 ${CMAKE_SOURCE_DIR}/lib/gumbo_parser/include
)

# Include project directories
include_directories(${COMMON_INCLUDES})

add_subdirectory(src)

# Unit tests
# include(CTest)
# 
# if (test)
#   add_subdirectory(${CMAKE_SOURCE_DIR}/lib/gtest-1.7.0)
#   add_subdirectory(test)
# endif()
