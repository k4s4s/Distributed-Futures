add_executable(blackscholes blackscholes.cpp)

target_link_libraries (blackscholes ${EXTRA_LIBS})

if(MPI_COMPILE_FLAGS)
  set_target_properties(blackscholes PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(blackscholes PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()