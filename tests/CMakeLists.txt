add_executable(hello_world hello.cpp)
add_executable(power power.cpp)
add_executable(factorial factorial.cpp)
add_executable(fibonacci fib.cpp)
add_executable(quicksort quicksort.cpp)

add_executable(ping_pong ping_pong.cpp)
add_executable(benchmark1 benchmark1.cpp)
add_executable(lu kernels.cpp lu.cpp)
add_executable(lu_plasma lu_plasma.cpp)
add_executable(lu_threads kernels.cpp lu_threads.cpp)
add_executable(lu_boost kernels.cpp lu_boost.cpp)

set(EXTRA_LIBS ${EXTRA_LIBS} 
	-lboost_serialization 
	-lboost_mpi
	-L../src/futures	
	-L../src/mutex
	-lfutures
	-lmutex
	)
set(EXTRA_LIBS ${EXTRA_LIBS} ${MPI_LIBRARIES})

target_link_libraries (hello_world ${EXTRA_LIBS})
target_link_libraries (power ${EXTRA_LIBS})
target_link_libraries (factorial ${EXTRA_LIBS})
target_link_libraries (fibonacci ${EXTRA_LIBS})
target_link_libraries (quicksort ${EXTRA_LIBS})

target_link_libraries (ping_pong ${EXTRA_LIBS})
target_link_libraries (benchmark1 ${EXTRA_LIBS})
target_link_libraries (lu ${EXTRA_LIBS}
													-L/opt/intel/mkl/lib/intel64
    											-L/opt/intel/lib/intel64
-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread)
target_link_libraries (lu_plasma ${EXTRA_LIBS} -L/users/parall/falcou/dimitrios/libs/plasma/lib 
																				-L/opt/intel/mkl/lib/intel64
    																		-L/opt/intel/lib/intel64
-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lhwloc
-lplasma -lcoreblas -lplasma -lquark -llapack -llapacke -lblas -lm -lpthread -lgfortran)
target_link_libraries (lu_threads ${EXTRA_LIBS}
																				-L/opt/intel/mkl/lib/intel64
    																		-L/opt/intel/lib/intel64
-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lhwloc -lpthread)
target_link_libraries (lu_boost ${EXTRA_LIBS}
																				-L/opt/intel/mkl/lib/intel64
    																		-L/opt/intel/lib/intel64
-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -L/usr/local/lib/ -lpthread -lboost_thread -lboost_system)
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
  set_target_properties(benchmark1 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
  set_target_properties(lu PROPERTIES
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
  set_target_properties(benchmark1 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
  set_target_properties(lu PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

