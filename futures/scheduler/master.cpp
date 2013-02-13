
#include "master.hpp"
#include "../common.hpp"
#include <stdlib.h>

#define MASTER 0

using namespace futures;
using namespace scheduler;

Master::Master(communication::CommInterface *_comm) {
    comm = _comm;
    id = comm->get_procId();
    nprocs = comm->size();
		status_mem = comm->new_shared_space(sizeof(ProcStatus));
		status_lock = comm->new_lock();
		//task_queue = new taskQueue();
		task_stack = new taskStack(comm);
    this->set_status(RUNNING);
};

Master::~Master() {
		delete status_mem;
    delete status_lock;
		//delete task_queue;
		delete task_stack;
};


void Master::set_status(ProcStatus status) {
		status_mem->put(&status, id, 1, 0, MPI_INT);
};

int Master::getId() {
	return id;
};

ProcStatus Master::get_status(int _id) {
		ProcStatus status;
		status_mem->get(&status, id, 1, 0, MPI_INT);	
		return status;
};

bool Master::terminate() {
    ProcStatus worker_status;
		if(nprocs == 1) return true; //only self to check, so just terminate
    for(int i=1; i < nprocs; i++) {
				status_mem->get(&worker_status, i, 1, 0, MPI_INT);
        if(worker_status == RUNNING) break;
        else if(i == nprocs-1) { //set your status to terminated
            ProcStatus status = TERMINATED;
						status_mem->put(&status, id, 1, 0, MPI_INT);
            return true;
        }
    }
    return false;
};

bool Master::available(int dst_id) {
	//return !task_queue->is_full(dst_id);
	return !task_stack->is_full(dst_id);
};

bool Master::send_job(int dst_id, _stub *job) { 
	//return task_queue->enqueue(dst_id, job);
	return task_stack->push(dst_id, job);
};

_stub *Master::get_job() { 
	//return task_queue->dequeue(id);
	return task_stack->pop(id);
};

bool Master::has_job() {
	//return !task_queue->is_empty(id);
	return !task_stack->is_empty(id);
};

