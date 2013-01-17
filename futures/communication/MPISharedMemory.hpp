
#ifndef _MPISHAREDMEMORY_H
#define _MPISHAREDMEMORY_H

#include <mpi.h>
#include "MPIComm.hpp"
#include "MPIMutex.hpp"
#include "mpi_details.hpp"

namespace futures {
namespace communication {

class MPI_Shared_memory {
private:
	MPI_Win data_win;
	MPIMutex *data_lock;
	void *shared_memory;
	unsigned int curr_index;
public:
	MPI_Shared_memory();
	~MPI_Shared_memory();
	unsigned int allocate(unsigned int size);
	void free(MPI_Shared_data *sharedData);
	MPI_Win get_data_window();
	MPIMutex *get_data_lock();
};


}
}

#endif
