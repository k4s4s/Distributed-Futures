
#ifndef _MPISHAREDMEMORY_H
#define _MPISHAREDMEMORY_H

#include <mpi.h>
#include "MPIMutex.hpp"
#include "mpi_details.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <cassert>
#include <list>
#include <map>

#define PAGE_SIZE_8b 8
#define PAGE_SIZE_128b 128
#define PAGE_SIZE_1kb 1024
#define PAGE_SIZE_4kb 4096
#define PAGE_SIZE_OTHER -1

#define SHARED_MEMORY_SIZE 2*1024
#define STATUS_OFFSET 0
#define AR_SIZE_OFFSET sizeof(int)
#define DATA_OFFSET sizeof(int)+sizeof(int)
#define NUM_OF_PAGES(x, n) ((x%n == 0)?(x/n):((x/n)+1)) 
#define PAGE_SIZE(x, n) \
	if(n < PAGE_SIZE_8b) x = PAGE_SIZE_8b; \
	else if(n < PAGE_SIZE_128b) x = PAGE_SIZE_128b; \
	else if(n < PAGE_SIZE_1kb) x = PAGE_SIZE_128b; \
	else if(n < PAGE_SIZE_4kb) x = PAGE_SIZE_4kb; \
	else x = size;

#define MAP_INDEX(x) (x > PAGE_SIZE_4kb)?(-1):x

namespace futures {
namespace communication {

struct Shared_pointer {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
      ar & base_address & size & num_of_pages & actual_size & page_size;
  };
public:
	int base_address;
	int size;
	int num_of_pages;
	int actual_size; //size*num_of_pages
	int page_size;
};

struct compare : public std::binary_function<Shared_pointer, Shared_pointer, bool>
{
	inline bool operator()(const Shared_pointer& a, const Shared_pointer& b)
	{
		return a.base_address < b.base_address;
	}
};

struct memory_pages {
public:
	MPI_Win data_win;
	void* shared_memory;
	std::list<Shared_pointer> free_list;
	int page_size;
	int total_size;
	memory_pages(int _page_size);
	~memory_pages();
};

class MPI_Shared_memory {
private:
	MPI_Win data_win;
	MPIMutex *data_lock;
	std::map<int, memory_pages*> freeLists;
	/*insert in sorted fashion with binary search*/
	void list_insert(std::list<Shared_pointer>& list, Shared_pointer& element);
	void print_list(std::list<Shared_pointer>& list);
public:
	MPI_Shared_memory();
	~MPI_Shared_memory();
	Shared_pointer allocate(unsigned int size);
	void free(Shared_pointer ptr);
	MPI_Win get_data_window(Shared_pointer ptr);
	MPIMutex *get_data_lock();
};


}
}

#endif
