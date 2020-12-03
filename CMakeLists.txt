cmake_minimum_required(VERSION 3.0.0)
project(Project2 VERSION 0.1.0)

include(CTest)
enable_testing()

# C++ Version 17
set(CMAKE_CXX_STANDARD 17)

# Required Threads
find_package(Threads REQUIRED)

# Creates the Executable
add_executable(Project2 src/main.cpp src/process.cpp src/pipe.cpp)

# Links Threads to the Executable as a Private object
target_link_libraries(Project2 PRIVATE Threads::Threads)

# Compiles the find_correct_word executable
add_executable(find_correct_word find_correct_word.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
