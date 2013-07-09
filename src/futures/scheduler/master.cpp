
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
		status_mem->put(status, id, 1, 0);
};

int Master::getId() {
	return id;
};

ProcStatus Master::get_status(int _id) {
		ProcStatus status;
		status = status_mem->get<ProcStatus>(id, 1, 0);
		return status;
};

#if 0
bool Master::terminate() {
		START_TIMER("terminate_time");
    ProcStatus worker_status;
		if(nprocs == 1) return true; //only self to check, so just terminate
    for(int i=1; i < nprocs; i++) {
				worker_status = status_mem->get<ProcStatus>(i, 1, 0);
        if(worker_status == RUNNING) break;
        else if(i == nprocs-1) { //set your status to terminated
            ProcStatus status = TERMINATED;
						status_mem->put(status, id, 1, 0);
						STOP_TIMER("terminate_time");
            return true;
        }
    }
		STOP_TIMER("terminate_time");
    return false;
};

#elif 1

bool Master::terminate() {
		START_TIMER("terminate_time");
    ProcStatus worker_status;
		if(nprocs == 1) return true; //only self to check, so just terminate
		//check if everyone is done
    for(int i=1; i < nprocs; i++) {
				worker_status = status_mem->get<ProcStatus>(i, 1, 0);
        if(worker_status == RUNNING) { 
					STOP_TIMER("terminate_time");
					return false;
				}
    }
		//if everyone is done, set their status and yours
    for(int i=1; i < nprocs; i++) {
      ProcStatus status = TERMINATED;
			status_mem->put(status, i, 1, 0);
    }	
    ProcStatus status = TERMINATED;
		status_mem->put(status, id, 1, 0);
		STOP_TIMER("terminate_time");
    return true;
};

#else

bool Master::terminate() {
	return true;
}

#endif

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
	//return !task_stack->is_empty(id);
	return false;
};

