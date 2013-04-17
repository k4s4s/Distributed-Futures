
#include "SharedMemoryManager.hpp"
#include "../common.hpp"
#include <algorithm>
#include <cstdlib>

using namespace futures;
using namespace mem;

/*** memory pages implementation ***/
memory_pages::memory_pages(communication::CommInterface *_comm, int _page_size) {
	comm = _comm;	
	page_size = _page_size;

	if(page_size != PAGE_SIZE_OTHER) //attention!
			total_size = Shared_Memory_manager_SIZE*page_size;
	else
			total_size = Shared_Memory_manager_SIZE*1024*8;
	
	shared_address_space = comm->new_shared_space(total_size);

	Shared_pointer free_mem;
	free_mem.base_address = 0;
	free_mem.size = free_mem.actual_size = total_size;
	free_mem.num_of_pages = NUM_OF_PAGES(free_mem.size, page_size);
	free_mem.page_size = page_size;
	free_list.push_front(free_mem);
	DPRINT_VAR("\t\tShare Mem:created shared memory of ", total_size);
	//std::cout << "total_size:" << total_size << std::endl;
};

memory_pages::~memory_pages() {
	delete shared_address_space;
};

/*** Shared_Memory_manager implementation ***/
Shared_Memory_manager::Shared_Memory_manager(communication::CommInterface *_comm) {

	comm = _comm;

	memory_pages* _8bpages = new memory_pages(comm, PAGE_SIZE_8b);
	memory_pages* _128bpages = new memory_pages(comm, PAGE_SIZE_128b);
	memory_pages* _1kbpages = new memory_pages(comm, PAGE_SIZE_1kb);
	memory_pages* _4kbpages = new memory_pages(comm, PAGE_SIZE_4kb);
	memory_pages* _otherpages = new memory_pages(comm, PAGE_SIZE_OTHER);

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

	mem_lock = comm->new_lock();
};

Shared_Memory_manager::~Shared_Memory_manager() {
	//TODO:free list
	std::map<int, memory_pages*>::iterator it;
	for(it = freeLists.begin(); it != freeLists.end(); it++) {
		delete it->second;
	}
	delete mem_lock;
};

void Shared_Memory_manager::list_insert(std::list<Shared_pointer>& list, 
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

void Shared_Memory_manager::print_list(std::list<Shared_pointer> &list) {
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

/*Add type here somewhere, so that we allocate different sizes for primitive types 
	and objects */
Shared_pointer Shared_Memory_manager::allocate(int id, unsigned int size) {
	size += DATA_OFFSET+sizeof(int);
	int page_size;
	DPRINT_VAR("\t\t\tShared Mem:Trying to allocate memory on ", id);
	PAGE_SIZE(page_size, size);
	std::map<int, memory_pages*>::iterator freeLists_it, best_fit_it;
	best_fit_it = freeLists_it = freeLists.find(MAP_INDEX(page_size));
	Shared_pointer ptr;
	bool first_try = true;
	while(freeLists_it != freeLists.end()) {
		ptr.base_address = ptr.size = ptr.num_of_pages = 0;
		ptr.page_size = page_size;
		int pages_needed = NUM_OF_PAGES(size, page_size);
		/*find a large enough space, with first fit*/
		std::list<Shared_pointer>::iterator it;
		for(it=freeLists_it->second->free_list.begin(); it!=freeLists_it->second->free_list.end(); ++it) {
			DPRINT_VAR("\t\t\tShared Mem:Free Mem:", (*it).actual_size);
			if((*it).actual_size >= pages_needed*page_size) {
				DPRINT_VAR("\t\t\tShared Mem:Allocated ", pages_needed*page_size);
				ptr.node_id = id;
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
			DPRINT_MESSAGE("\t\t\tShared Mem:Trying another page size");	
			//try to allocate space in another page size space
			if(first_try) {
				freeLists_it = freeLists.begin();
				first_try = false;
			}
			else {
				freeLists_it++;
			}
			//skip best fit, we have already checked it
			if(freeLists_it == best_fit_it)
				freeLists_it++;
			page_size = (*freeLists_it).first;
			pages_needed = NUM_OF_PAGES(size, page_size);
		}
		else 
			break; //memory allocated succesfully
	}
	if(ptr.size == 0) {
		PRINT_COUNTER("total_memory_needed");
	}
	assert(ptr.size != 0); //TODO: if this fails, throw an exception and run on self
	INCREASE_COUNTER("total_memory_needed", ptr.actual_size);
	return ptr;
};

void Shared_Memory_manager::free(int id, Shared_pointer& ptr) {
	int zero = 0;
	std::map<int, memory_pages*>::iterator freeLists_it;
	freeLists_it = freeLists.find(MAP_INDEX(ptr.page_size));
	freeLists_it->second->shared_address_space->put(zero, id, 1, ptr.base_address);
	list_insert(freeLists_it->second->free_list, ptr);
};

communication::Shared_Address_space* Shared_Memory_manager::get_shared_space(Shared_pointer& ptr) {
	std::map<int, memory_pages*>::iterator freeLists_it;
	freeLists_it = freeLists.find(MAP_INDEX(ptr.page_size));
	int index = MAP_INDEX(ptr.page_size);
	return freeLists_it->second->shared_address_space;
};



