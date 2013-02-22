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

include_directories(mutex)
include_directories(futures)
include_directories(futures/communication)
include_directories(futures/mem)
include_directories(futures/scheduler)
include_directories(futures/stats)

add_definitions(-O3 -std=c++0x)

add_subdirectory(src/mutex)
add_subdirectory(src/futures)
add_subdirectory(tests)

#clean-all command
add_custom_target(clean-cmake-files
   COMMAND ${CMAKE_COMMAND} -P clean-all.cmake
)

set(cmake_generated ${CMAKE_BINARY_DIR}/CMakeCache.txt
                    ${CMAKE_BINARY_DIR}/cmake_install.cmake  
                    ${CMAKE_BINARY_DIR}/Makefile
                    ${CMAKE_BINARY_DIR}/CMakeFiles
)

foreach(file ${cmake_generated})

  if (EXISTS ${file})
     file(REMOVE_RECURSE ${file})
  endif()

endforeach(file)

