
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
	free_mem.size = free_mem.actual_size = SHARED_MEMORY_SIZE;
	free_mem.num_of_pages = NUM_OF_PAGES(free_mem.size);
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
	//find last element in list before high, to check if merging is possible
	for(it = list.begin(); list.size() > 1; it++) {
		std::list<Shared_pointer>::iterator nxt;
		nxt = it;
		nxt++;
		if(nxt == high) break;
	};
	//check if we can merge with previout entry
	if((*it).base_address+(*it).actual_size == element.base_address) {
		//merge two elements
		//(*it).size += element.actual_size;
		(*it).actual_size += element.actual_size;
		(*it).num_of_pages += element.num_of_pages;
		//also check if merging with next element in list is possible
		if((*it).base_address+(*it).actual_size == (*high).base_address) {
			(*it).actual_size += (*high).actual_size;
			(*it).num_of_pages += (*high).num_of_pages;
			list.erase(high);
		}
	} 
	else if(high != free_list.end() && (*high).base_address == element.base_address+element.actual_size) {
		//merge two elements
		//(*high).size += element.actual_size;
		(*high).actual_size += element.actual_size;
		(*high).num_of_pages += element.num_of_pages;
		(*high).base_address = element.base_address;
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
		DPRINT_VAR("\t\t\tShare Mem:list:", (*it).num_of_pages);
		DPRINT_VAR("\t\t\tShare Mem:list:", (*it).actual_size);
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
	ptr.base_address = ptr.size = ptr.num_of_pages = 0;
	int pages_needed = NUM_OF_PAGES(size);
	for(it=free_list.begin(); it!=free_list.end(); ++it) {
		if((*it).num_of_pages >= pages_needed) {
			DPRINT_VAR("\t\t\tShared Mem:Allocated ", pages_needed*PAGE_SIZE);
			ptr.base_address = (*it).base_address;
			ptr.size = size;
			ptr.num_of_pages = pages_needed;
			ptr.actual_size = pages_needed*PAGE_SIZE;
			(*it).base_address += pages_needed*PAGE_SIZE;
			(*it).size -= pages_needed*PAGE_SIZE;
			(*it).num_of_pages -= pages_needed;
			(*it).actual_size -= ptr.actual_size;
			break;
		}
	}
	if(ptr.size == 0) {
		DPRINT_VAR("\t\t\tShared Mem:Could not allocate ",size);	
	}
	assert(ptr.size != 0); //TODO: if this fails, throw an exception and run on self
	return ptr;
};

void MPI_Shared_memory::free(Shared_pointer ptr) {
	char *_offset_address = static_cast<char*>(shared_memory)+ptr.base_address;
	bzero(_offset_address, sizeof(int));
	list_insert(free_list, ptr);
};

MPI_Win MPI_Shared_memory::get_data_window() {
	return data_win;
};
	
MPIMutex* MPI_Shared_memory::get_data_lock() {
	return data_lock;
};

