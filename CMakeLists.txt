#Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.6)
#Name your project here
project(mpi-f)


set(LIB_INSTALL_DIR, CMAKE_INSTALL_PREFIX)

# Require MPI for this project:
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Boost REQUIRED COMPONENTS mpi)

find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
add_custom_target(doc
  										COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
  										SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)
endif(DOXYGEN_FOUND)

option(debug "debug" 0)
option(stats "stats" OFF)

if(debug MATCHES 1)
	add_definitions(-g)
endif(debug MATCHES 1)

if(debug MATCHES 2)
	add_definitions(-g -DDEBUG)
endif(debug MATCHES 2)

if(stats MATCHES ON)
	add_definitions(-DSTATS)
endif(stats MATCHES ON)

include_directories(mutex)
include_directories(futures)
include_directories(futures/communication)
include_directories(futures/mem)
include_directories(futures/scheduler)
include_directories(futures/stats)

add_definitions(-I/users/parall/falcou/dimitrios/libs/plasma/include -I/opt/intel/mkl/include -O3 -std=c++0x -lpthread)
add_definitions(-DV_MPI)

add_subdirectory(src/mutex)
add_subdirectory(src/futures)
add_subdirectory(tests)

