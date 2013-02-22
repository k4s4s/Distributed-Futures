add_library(futures STATIC
	futures_environment.cpp 
	communication/commManager.cpp
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

install(FILES 
	${CMAKE_SOURCE_DIR}/futures/future_fwd.hpp
	${CMAKE_SOURCE_DIR}/futures/communication/communication.hpp
	${CMAKE_SOURCE_DIR}/futures/communication/commManager.hpp
	${CMAKE_SOURCE_DIR}/futures/communication/MPIComm.hpp
	${CMAKE_SOURCE_DIR}/futures/mem/SharedMemoryManager.hpp
	${CMAKE_SOURCE_DIR}/futures/scheduler/process.hpp
	${CMAKE_SOURCE_DIR}/futures/scheduler/scheduler.hpp
	${CMAKE_SOURCE_DIR}/futures/scheduler/RR.hpp
	${CMAKE_SOURCE_DIR}/futures/scheduler/taskStack.hpp
	${CMAKE_SOURCE_DIR}/futures/scheduler/schedManager.hpp
	${CMAKE_SOURCE_DIR}/futures/stats/stats.hpp
	${CMAKE_SOURCE_DIR}/futures/apply_function.hpp
	${CMAKE_SOURCE_DIR}/futures/details.hpp
	${CMAKE_SOURCE_DIR}/futures/common.hpp
	${CMAKE_SOURCE_DIR}/futures/futures_environment.hpp
	${CMAKE_SOURCE_DIR}/futures/future.hpp
	${CMAKE_SOURCE_DIR}/futures/futures.hpp
DESTINATION include)

install(TARGETS futures DESTINATION lib)

