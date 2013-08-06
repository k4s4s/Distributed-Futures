#Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.6)
#Name your project here
project(benchmarks)


set(LIB_INSTALL_DIR, CMAKE_INSTALL_PREFIX)

# Require MPI for this project:
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Boost REQUIRED COMPONENTS mpi)

option(debug "debug" ON)
option(stats "stats" OFF)

if(debug MATCHES ON)
	add_definitions(-g)
endif(debug MATCHES ON)

if(stats MATCHES ON)
	add_definitions(-DSTATS)
endif(stats MATCHES ON)

include_directories(../futures)
include_directories(../mutex)

add_definitions(-I$ENV{BOOST_ROOT}/include -O3 -std=c++0x)

set(EXTRA_LIBS ${EXTRA_LIBS} 
	${MPI_LIBRARIES}
	-L$ENV{BOOST_ROOT}/lib
	-lboost_serialization 
	-lboost_mpi
	-L../../src/futures	
	-L../../src/mutex
	-lfutures
	-lmutex
	)

add_subdirectory(tiledLU)
add_subdirectory(blackscholes)
