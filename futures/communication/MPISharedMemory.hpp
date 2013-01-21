
#ifndef _MPISHAREDMEMORY_H
#define _MPISHAREDMEMORY_H

#include <mpi.h>
#include "MPIMutex.hpp"
#include "mpi_details.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <cassert>

#define SHARED_MEMORY_SIZE 1000000
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

class MPI_Shared_memory {
private:
	MPI_Win data_win;
	MPIMutex *data_lock;
	void *shared_memory;
	unsigned int curr_index;
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
