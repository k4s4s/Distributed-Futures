add_library(futures STATIC
	communication/MPIComm.cpp
	mem/SharedMemoryManager.cpp
	scheduler/taskStack.cpp
	scheduler/master.cpp
	scheduler/worker.cpp
	scheduler/RR.cpp
	scheduler/schedManager.cpp
	stats/stats.cpp
	futures_environment.cpp)

target_link_libraries(futures ${MPI_LIBRARIES} 	
  -L$ENV{BOOST_ROOT}/lib
	-lboost_mpi
	-L../src/mutex
	-lmutex) 

install(DIRECTORY ${CMAKE_SOURCE_DIR}/futures DESTINATION include)

install(TARGETS futures DESTINATION lib)

