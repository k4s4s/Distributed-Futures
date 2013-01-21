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
    ProcStatus *status;
		//taskQueue *task_queue;
		taskStack *task_stack;
    int id;
    int nprocs;
public:
    Master();
    ~Master();
		int getId();
    void set_status(ProcStatus status);
		ProcStatus get_status(int _id);
    bool terminate();
		bool available(int dst_id);
		bool send_job(int dst_id, _stub *job);
		_stub *get_job();
		bool has_job();
};

}
}

#endif
