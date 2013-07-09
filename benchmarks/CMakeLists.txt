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

if(debug MATCHES ON)
	add_definitions(-g)
endif(debug MATCHES ON)

include_directories(../futures)
include_directories(../mutex)
include_directories(/opt/intel/mkl/include)

add_definitions(-O3 -std=c++0x)

set(EXTRA_LIBS ${EXTRA_LIBS} 
	${MPI_LIBRARIES}
	-lboost_serialization 
	-lboost_mpi
	-L../../src/futures	
	-L../../src/mutex
	-lfutures
	-lmutex
	)

add_subdirectory(tiledLU)
add_subdirectory(blackscholes)