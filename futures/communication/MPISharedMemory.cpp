
#include "MPISharedMemory.hpp"
#include "../common.hpp"
#include <algorithm>

using namespace futures;
using namespace communication;

/*** MPI_Shared_memory implementation ***/
MPI_Shared_memory::MPI_Shared_memory() {
	MPI_Alloc_mem(SHARED_MEMORY_SIZE, MPI_INFO_NULL, &shared_memory);
  MPI_Win_create(shared_memory, SHARED_MEMORY_SIZE, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &data_win);
	data_lock = new MPIMutex(MPI_COMM_WORLD);
	Shared_pointer free_mem;
	free_mem.base_address = 0;
	free_mem.size = SHARED_MEMORY_SIZE;
	free_list.push_front(free_mem);
};

MPI_Shared_memory::~MPI_Shared_memory() {
  MPI_Win_free(&data_win);
  MPI_Free_mem(shared_memory);
	delete data_lock;
};

void MPI_Shared_memory::list_insert(std::list<Shared_pointer>& list, Shared_pointer& element) {
	if(list.size() == 0) {
		list.push_front(element);
		return;
	}
  std::list<Shared_pointer>::iterator it, high;
	high = upper_bound(list.begin(), list.end(), element, compare());
	
	for(it = list.begin(); it != high; ++it);
	//check if we can merge with previout entry
	if((*it).size+1 == element.base_address) {
		//merge two elements
		(*it).size += element.size;
	} 
	if(high != free_list.end() && (*high).base_address == element.size+1) {
		//merge two elements
		element.size += (*high).size;
	}
	else {
		free_list.insert(it, element);	
	}
	return;
}

void MPI_Shared_memory::print_list(std::list<Shared_pointer> &list) {
	std::list<Shared_pointer>::iterator it;
	DPRINT_VAR("\t\t\tShare Mem:list:", list.size());
	for(it = list.begin(); it != list.end(); ++it) {
		DPRINT_VAR("\t\t\tShare Mem:list:", (*it).base_address);	
		DPRINT_VAR("\t\t\tShare Mem:list:", (*it).size);
		DPRINT_MESSAGE("\t\t\t============");
	}
	return;
};

Shared_pointer MPI_Shared_memory::allocate(unsigned int size) {
	size += DATA_OFFSET;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	DPRINT_VAR("\t\t\tShared Memory:Trying to allocate memory on ", rank);
	/*find a large enough space, with first fit*/
	std::list<Shared_pointer>::iterator it;
	Shared_pointer ptr;
	ptr.base_address = ptr.size = 0;
	for(it=free_list.begin(); it!=free_list.end(); ++it) {
		if((*it).size >= size) {
			DPRINT_VAR("\t\t\tShared Mem:Allocated ", size);
			ptr.base_address = (*it).base_address;
			ptr.size = size;
			(*it).base_address += size+1;
			(*it).size -= size;
			break;
		}
	}
	if(ptr.base_address == ptr.size) {
		DPRINT_VAR("\t\t\tShared Mem:Could not allocate ",size);	
	}
	//print_list(free_list);
	assert(ptr.base_address != ptr.size); //TODO: if this fails, throw an exception and run on self
	return ptr;
};

void MPI_Shared_memory::free(Shared_pointer ptr) {
	list_insert(free_list, ptr);
};

MPI_Win MPI_Shared_memory::get_data_window() {
	return data_win;
};
	
MPIMutex* MPI_Shared_memory::get_data_lock() {
	return data_lock;
};
