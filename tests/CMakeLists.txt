add_executable(hello_world hello.cpp)
add_executable(power power.cpp)
add_executable(factorial factorial.cpp)
add_executable(fibonacci fib.cpp)
add_executable(quicksort quicksort.cpp)
add_executable(test test.cpp)

add_executable(ping_pong ping_pong.cpp)
add_executable(bench_issue_container bench_issue_container.cpp)
add_executable(bench_issue_scalar bench_issue_scalar.cpp)

set(EXTRA_LIBS ${EXTRA_LIBS} 
	-lboost_serialization 
	-lboost_mpi
	-L../src/futures	
	-L../src/mutex
	-lfutures
	-lmutex
	)

set(EXTRA_LIBS ${EXTRA_LIBS} ${MPI_LIBRARIES})

target_link_libraries (test ${EXTRA_LIBS})
target_link_libraries (hello_world ${EXTRA_LIBS})
target_link_libraries (power ${EXTRA_LIBS})
target_link_libraries (factorial ${EXTRA_LIBS})
target_link_libraries (fibonacci ${EXTRA_LIBS})
target_link_libraries (quicksort ${EXTRA_LIBS})
target_link_libraries (ping_pong ${EXTRA_LIBS})
target_link_libraries (bench_issue_container ${EXTRA_LIBS})
target_link_libraries (bench_issue_scalar ${EXTRA_LIBS})

if(MPI_COMPILE_FLAGS)
  set_target_properties(hello_world PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(power PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(factorial PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(fibonacci PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(quicksort PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")

  set_target_properties(ping_pong PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(bench_issue_container PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(bench_issue_scalar PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(test PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(hello_world PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(power PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(factorial PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(fibonacci PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(quicksort PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")

  set_target_properties(ping_pong PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(bench_issue_container PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(bench_issue_scalar PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(test PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

