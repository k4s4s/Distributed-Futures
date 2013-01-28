
#include "MPISharedMemory.hpp"
#include "../common.hpp"
#include <algorithm>
#include <cstdlib>

using namespace futures;
using namespace communication;

/*** memory pages implementation ***/
memory_pages::memory_pages(int _page_size) {
	page_size = _page_size;

	if(page_size != PAGE_SIZE_OTHER) //attention!
			total_size = SHARED_MEMORY_SIZE*page_size;
	else
			total_size = SHARED_MEMORY_SIZE*1024*8;
	MPI_Alloc_mem(total_size, MPI_INFO_NULL, &shared_memory);
  MPI_Win_create(shared_memory, total_size, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &data_win);

	Shared_pointer free_mem;
	free_mem.base_address = 0;
	free_mem.size = free_mem.actual_size = total_size;
	free_mem.num_of_pages = NUM_OF_PAGES(free_mem.size, page_size);
	free_mem.page_size = page_size;
	free_list.push_front(free_mem);
};

memory_pages::~memory_pages() {
  MPI_Win_free(&data_win);
  MPI_Free_mem(shared_memory);
};

/*** MPI_Shared_memory implementation ***/
MPI_Shared_memory::MPI_Shared_memory() {

	memory_pages* _8bpages = new memory_pages(PAGE_SIZE_8b);
	memory_pages* _128bpages = new memory_pages(PAGE_SIZE_128b);
	memory_pages* _1kbpages = new memory_pages(PAGE_SIZE_1kb);
	memory_pages* _4kbpages = new memory_pages(PAGE_SIZE_4kb);
	memory_pages* _otherpages = new memory_pages(PAGE_SIZE_OTHER);

	freeLists.insert(
		std::pair<int, memory_pages*>(PAGE_SIZE_8b, _8bpages));
	freeLists.insert(
		std::pair<int, memory_pages*>(PAGE_SIZE_128b, _128bpages));
	freeLists.insert(
		std::pair<int, memory_pages*>(PAGE_SIZE_1kb, _1kbpages));
	freeLists.insert(
		std::pair<int, memory_pages*>(PAGE_SIZE_4kb, _4kbpages));
	freeLists.insert(
		std::pair<int, memory_pages*>(PAGE_SIZE_OTHER, _otherpages));

	data_lock = new MPIMutex(MPI_COMM_WORLD);
};

MPI_Shared_memory::~MPI_Shared_memory() {
	//TODO:free list
	std::map<int, memory_pages*>::iterator it;
	for(it = freeLists.begin(); it != freeLists.end(); it++) {
		delete it->second;
	}
	delete data_lock;
};

void MPI_Shared_memory::list_insert(std::list<Shared_pointer>& list, 
																	Shared_pointer& element) {
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
	else if(high != list.end() && (*high).base_address == element.base_address+element.actual_size) {
		//merge two elements
		//(*high).size += element.actual_size;
		(*high).actual_size += element.actual_size;
		(*high).num_of_pages += element.num_of_pages;
		(*high).base_address = element.base_address;
	}
	else {
		list.insert(it, element);	
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
	int page_size;
	PAGE_SIZE(page_size, size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	DPRINT_VAR("\t\t\tShared Mem:Trying to allocate memory on ", rank);
	std::map<int, memory_pages*>::iterator freeLists_it;
	freeLists_it = freeLists.find(MAP_INDEX(page_size));
	Shared_pointer ptr;
	ptr.base_address = ptr.size = ptr.num_of_pages = 0;
	ptr.page_size = page_size;
	int pages_needed = NUM_OF_PAGES(size, page_size);
	/*find a large enough space, with first fit*/
	std::list<Shared_pointer>::iterator it;
	for(it=freeLists_it->second->free_list.begin(); it!=freeLists_it->second->free_list.end(); ++it) {
		DPRINT_VAR("\t\t\tShared Mem:Free Mem:", (*it).actual_size);
		if((*it).actual_size >= pages_needed*page_size) {
			DPRINT_VAR("\t\t\tShared Mem:Allocated ", pages_needed*page_size);
			ptr.base_address = (*it).base_address;
			ptr.size = size;
			ptr.num_of_pages = pages_needed;
			ptr.actual_size = pages_needed*page_size;
			(*it).base_address += pages_needed*page_size;
			(*it).size -= pages_needed*page_size;
			(*it).num_of_pages -= pages_needed;
			(*it).actual_size -= ptr.actual_size;
			break;
		}
	}
	if(ptr.size == 0) {
		DPRINT_VAR("\t\t\tShared Mem:Could not allocate ", pages_needed*page_size);	
	}
	assert(ptr.size != 0); //TODO: if this fails, throw an exception and run on self
	return ptr;
};

void MPI_Shared_memory::free(Shared_pointer ptr) {
	std::map<int, memory_pages*>::iterator freeLists_it;
	freeLists_it = freeLists.find(MAP_INDEX(ptr.page_size));
	char *_offset_address = 
		static_cast<char*>(freeLists_it->second->shared_memory)+ptr.base_address;
	bzero(_offset_address, sizeof(int));
	list_insert(freeLists_it->second->free_list, ptr);
};

MPI_Win MPI_Shared_memory::get_data_window(Shared_pointer ptr) {
	std::map<int, memory_pages*>::iterator freeLists_it;
	freeLists_it = freeLists.find(MAP_INDEX(ptr.page_size));
	int index = MAP_INDEX(ptr.page_size);
	return freeLists_it->second->data_win;
};
	
MPIMutex* MPI_Shared_memory::get_data_lock() {
	return data_lock;
};

