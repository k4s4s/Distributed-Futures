#ifndef _WORKER_H
#define _WORKER_H

#include "process.hpp"
#include "mpi.h"
#include "MPIMutex.hpp"

namespace futures {
namespace scheduler {

class Worker : public Process {
private:
	MPI_Comm comm;
	MPI_Win status_win;
	MPIMutex *status_lock;
	int id;
public:
	Worker();
	~Worker();
	void set_status(ProcStatus status);
	bool terminate();
};

}
}

#endif
