cmake_minimum_required(VERSION 3.12)
# CMake Deprecation Warning at CMakeLists.txt:1 (cmake_minimum_required):
#   Compatibility with CMake < 2.8.12 will be removed from a future version of
#   CMake.
# add_link_options introduced in cmake v3.12

project(make_example VERSION 1.0.0 DESCRIPTION "cmake example")


# set(PROFILING "-pg")
set(SANITIZER 1)
set(DEBUG "-g" "-gdwarf-2")

if (SANITIZER)
set(SANITIZER_ARGS_COMPILE "-fsanitize=address" "-fsanitize=undefined")
set(SANITIZER_ARGS_LINK "-fsanitize=undefined")
endif()

if(PROFILING AND SANITIZER_ARGS_COMPILE)
MESSAGE(FATAL_ERROR "Cannot do profiling with libasan, unset PROFILING or SANITIZER")
endif()

# https://gist.github.com/rkitover/38c644a3eaa54e799d940ec2a7a3c4d6
if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
	add_compile_options(-fdiagnostics-color=always)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
	add_compile_options(-fcolor-diagnostics)
endif()

# EXECUTABLE
set(TARGET_NAME main)
add_compile_options(-Wall -Wextra -pedantic -Werror ${SANITIZER_ARGS_COMPILE} ${PROFILING} ${DEBUG})
add_link_options(${SANITIZER_ARGS_LINK} ${PROFILING} ${DEBUG})
# build executable, primary compile target
add_executable(${TARGET_NAME} main.cc)
target_include_directories(${TARGET_NAME} PRIVATE "./include")

if (SANITIZER)
# find_library(LIBASAN libasan.so)
# https://stackoverflow.com/questions/64646938/how-to-obtain-the-libasan-so-path-with-cmake
# https://cmake.org/cmake/help/latest/command/execute_process.html
execute_process(COMMAND ${CMAKE_C_COMPILER} -print-file-name=libasan.so
				OUTPUT_VARIABLE LIBASAN_FULL_PATH
				RESULT_VARIABLE COMMAND_RESULT
				OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE(STATUS "COMMAND_RESULT: " ${COMMAND_RESULT})
MESSAGE(STATUS "LIBASAN_FULL_PATH: " ${LIBASAN_FULL_PATH})
# https://cmake.org/pipermail/cmake/2014-September/058647.html
# [CMake] TARGET_LINK_LIBRARIES with full path libraries
add_library(asan UNKNOWN IMPORTED)
set_property(TARGET asan PROPERTY IMPORTED_LOCATION "${LIBASAN_FULL_PATH}")
target_link_libraries(${TARGET_NAME} PRIVATE asan)
# Book: Modern CMake
# Do not use global functions: This includes link_directories , include_libraries , and similar.
# link_directories(${LIBASAN_PATH})
endif()
