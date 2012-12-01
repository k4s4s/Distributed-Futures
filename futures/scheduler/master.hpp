#ifndef _MASTER_H
#define _MASTER_H

#include "process.hpp"
#include "mpi.h"
#include "MPIMutex.hpp"

namespace futures {
namespace scheduler {

class Master : public Process {
private:
	MPI_Comm comm;
	MPI_Win	status_win;
	MPIMutex *status_lock;
	ProcStatus *status_vector;
	int id;
	int nprocs;
public:
	Master();
	~Master();
	void set_status(ProcStatus status);
	bool terminate();
};

}
}

#endif
