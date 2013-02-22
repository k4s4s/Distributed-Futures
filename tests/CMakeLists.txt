add_executable(fibonacci fib.cpp)

set(EXTRA_LIBS ${EXTRA_LIBS} 
	-lboost_serialization 
	-lboost_mpi
	-L../src/futures	
	-L../src/mutex
	-lfutures
	-lmutex
	)
set(EXTRA_LIBS ${EXTRA_LIBS} ${MPI_LIBRARIES})
target_link_libraries (fibonacci ${EXTRA_LIBS})

if(MPI_COMPILE_FLAGS)
  set_target_properties(fibonacci PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(fibonacci PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

