
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

#define PAGE_SIZE 128
#define SHARED_MEMORY_SIZE 2*1024*PAGE_SIZE //128MB
#define STATUS_OFFSET 0
#define AR_SIZE_OFFSET sizeof(int)
#define DATA_OFFSET sizeof(int)+sizeof(int)

namespace futures {
namespace communication {

struct Shared_pointer {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
      ar & base_address & size;
  };
public:
	int base_address;
	int size;
};

struct compare : public std::binary_function<Shared_pointer, Shared_pointer, bool>
{
	inline bool operator()(const Shared_pointer& a, const Shared_pointer& b)
	{
		return a.base_address < b.base_address;
	}
};

class MPI_Shared_memory {
private:
	MPI_Win data_win;
	MPIMutex *data_lock;
	void *shared_memory;
	unsigned int curr_index;
	std::list<Shared_pointer> free_list;
	std::list<Shared_pointer> allocated_list;
	/*insert in sorted fashion with binary search*/
	void list_insert(std::list<Shared_pointer>& list, Shared_pointer& element);
public:
	MPI_Shared_memory();
	~MPI_Shared_memory();
	Shared_pointer allocate(unsigned int size);
	void free(Shared_pointer ptr);
	MPI_Win get_data_window();
	MPIMutex *get_data_lock();
};


}
}

#endif
