add_library(mutex STATIC MPIMutex.cpp)

target_link_libraries(mutex ${MPI_LIBRARIES}) 

if(MPI_COMPILE_FLAGS)
  set_target_properties(mutex PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(mutex PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/mutex DESTINATION include)

install(TARGETS mutex DESTINATION lib)
