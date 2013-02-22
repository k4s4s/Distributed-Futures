
#ifndef _SHAREDMEMORY_H
#define _SHAREDMEMORY_H

#include "../communication/communication.hpp"
#include "../details.hpp"
#include <cassert>
#include <list>
#include <map>
#include <mutex.hpp>

#define PAGE_SIZE_8b 8
#define PAGE_SIZE_128b 128
#define PAGE_SIZE_1kb 1024
#define PAGE_SIZE_4kb 4096
#define PAGE_SIZE_OTHER -1

#define Shared_Memory_manager_SIZE 2*1024*10

#define STATUS_OFFSET 0
#define DATA_OFFSET sizeof(int)

#define NUM_OF_PAGES(x, n) ((x%n == 0)?(x/n):((x/n)+1)) 

#define PAGE_SIZE(x, n) \
	if(n < PAGE_SIZE_8b) x = PAGE_SIZE_8b; \
	else if(n < PAGE_SIZE_128b) x = PAGE_SIZE_128b; \
	else if(n < PAGE_SIZE_1kb) x = PAGE_SIZE_128b; \
	else if(n < PAGE_SIZE_4kb) x = PAGE_SIZE_4kb; \
	else x = size;

#define MAP_INDEX(x) (x > PAGE_SIZE_4kb)?(-1):x

namespace futures {
namespace mem {

struct Shared_pointer {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
      ar & base_address & size & num_of_pages & actual_size & page_size;
  };
public:
	int base_address;
	std::size_t size;
	int num_of_pages;
	std::size_t actual_size; //size*num_of_pages
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
	communication::CommInterface *comm;
	communication::Shared_Address_space *shared_address_space;
	std::list<Shared_pointer> free_list;
	int page_size;
	std::size_t total_size;
	memory_pages(communication::CommInterface *_comm, int _page_size);
	~memory_pages();
};

class Shared_Memory_manager {
private:
	communication::CommInterface *comm;
	mutex *mem_lock;
	std::map<int, memory_pages*> freeLists;
	/*insert in sorted fashion with binary search*/
	void list_insert(std::list<Shared_pointer>& list, Shared_pointer& element);
	void print_list(std::list<Shared_pointer>& list);
 communication::Shared_Address_space* get_shared_space(Shared_pointer& ptr); 
public:
	Shared_Memory_manager(communication::CommInterface *_comm);
	~Shared_Memory_manager();
	Shared_pointer allocate(int id, unsigned int size);
	void free(int id, Shared_pointer& ptr);
	template<typename T>
	void memset(T& data, int dst_id, Shared_pointer& ptr, int size, int offset);
	template<typename T>
	T memget(int src_id, Shared_pointer& ptr, int size, int offset);
};

//FIXME: I think size is incLuded in ptr, have to check if it is consistent though, else fix it
template<typename T>
void Shared_Memory_manager::memset(T& data, int dst_id, Shared_pointer& ptr, int size, int offset) {
		communication::Shared_Address_space *shared_space = get_shared_space(ptr);
    details::_put<T>()(shared_space, data, dst_id, size, ptr.base_address+offset,
											details::is_primitive_type<T>());
};

template<typename T>
T Shared_Memory_manager::memget(int src_id, Shared_pointer& ptr, int size, int offset) {
		communication::Shared_Address_space *shared_space = get_shared_space(ptr);
    return details::_get<T>()(shared_space, src_id, size, ptr.base_address+offset,
															details::is_primitive_type<T>());
};

}
}

#endif
