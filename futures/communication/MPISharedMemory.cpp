
#include "MPISharedMemory.hpp"

using namespace futures;
using namespace communication;

/*** MPI_Shared_memory implementation ***/
MPI_Shared_memory::MPI_Shared_memory() {
	MPI_Alloc_mem(SHARED_MEMORY_SIZE, MPI_INFO_NULL, &shared_memory);
  MPI_Win_create(shared_memory, SHARED_MEMORY_SIZE, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &data_win);
	data_lock = new MPIMutex(MPI_COMM_WORLD);
	curr_index = 0;
};

MPI_Shared_memory::~MPI_Shared_memory() {
  MPI_Win_free(&data_win);
  MPI_Free_mem(shared_memory);
	delete data_lock;
};

Shared_pointer MPI_Shared_memory::allocate(unsigned int size) {
	Shared_pointer ptr;
	ptr.base_address = curr_index;
	ptr.size = size+DATA_OFFSET;
	assert(curr_index+size+DATA_OFFSET < SHARED_MEMORY_SIZE); //TODO: if this fails, run on self
	curr_index+=size+DATA_OFFSET;
	return ptr;
};

void MPI_Shared_memory::free(Shared_pointer ptr) {};

MPI_Win MPI_Shared_memory::get_data_window() {
	return data_win;
};
	
MPIMutex* MPI_Shared_memory::get_data_lock() {
	return data_lock;
};
