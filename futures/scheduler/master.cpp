
#include "master.hpp"
#include "../communication/mpi_details.hpp"
#include "../common.hpp"
#include <stdlib.h>

#define MASTER 0

using namespace futures;
using namespace scheduler;

Master::Master() {
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &id);
    MPI_Comm_size(comm, &nprocs);
    MPI_Alloc_mem(sizeof(ProcStatus), MPI_INFO_NULL, &status);
    MPI_Win_create(status, 1, sizeof(ProcStatus), MPI_INFO_NULL, comm, &status_win);
    status_lock = new MPIMutex(comm);
		task_queue = new taskQueue();
    this->set_status(RUNNING);
};

Master::~Master() {
    MPI_Win_free(&status_win);
		MPI_Free_mem(status);
    delete status_lock;
		delete task_queue;
};


void Master::set_status(ProcStatus status) {
    communication::details::lock_and_put(&status, status, MPI_INT, id, 0, 1,
                                         MPI_INT, status_win, status_lock);
};

int Master::getId() {
	return id;
};

ProcStatus Master::get_status(int _id) {
		ProcStatus status;
    communication::details::lock_and_get(&status, 1, MPI_INT, id, 0, 1,
                                         MPI_INT, status_win, status_lock);		
		return status;
};

bool Master::terminate() {
    ProcStatus worker_status;
		if(nprocs == 1) return true; //only self to check, so just terminate
    for(int i=1; i < nprocs; i++) {
        communication::details::lock_and_get(&worker_status, 1, MPI_INT, i, 0, 1,
                                             MPI_INT, status_win, status_lock);
        if(worker_status == RUNNING) break;
        else if(i == nprocs-1) { //set your status to terminated
            ProcStatus status = TERMINATED;
            communication::details::lock_and_put(&status, 1, MPI_INT, MASTER, 0, 1,
                                                 MPI_INT, status_win, status_lock);
            return true;
        }
    }
    return false;
};

bool Master::available(int dst_id) {
	return !task_queue->is_full(dst_id);
};

bool Master::send_job(int dst_id, _stub *job) { 
	return task_queue->enqueue(dst_id, job);
};

_stub *Master::get_job() { 
	return task_queue->dequeue(id);
};


bool Master::has_job() {
	return !task_queue->is_empty(id);
};

