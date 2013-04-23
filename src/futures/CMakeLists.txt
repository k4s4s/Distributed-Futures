add_library(futures STATIC
	futures_environment.cpp 
	communication/MPIComm.cpp
	mem/SharedMemoryManager.cpp
	scheduler/schedManager.cpp
	scheduler/master.cpp
	scheduler/worker.cpp
	scheduler/RR.cpp
	scheduler/taskStack.cpp
	stats/stats.cpp)

target_link_libraries(futures ${MPI_LIBRARIES} 	
	-L../src/mutex
	-lmutex) 

if(MPI_COMPILE_FLAGS)
  set_target_properties(futures PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(futures PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/futures DESTINATION include)

install(TARGETS futures DESTINATION lib)

