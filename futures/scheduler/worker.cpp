
#include "worker.hpp"
#include "../common.hpp"

#define MASTER 0

using namespace futures;
using namespace scheduler;

Worker::Worker(communication::CommInterface *_comm) {
    int nprocs;
    comm = _comm;
    id = comm->get_procId();
		nprocs = comm->size();
		status_mem = comm->new_shared_space(sizeof(ProcStatus));
    status_lock = comm->new_lock();
		//task_queue = new taskQueue();
		task_stack = new taskStack(comm);
    this->set_status(IDLE);
};

Worker::~Worker() {
		delete status_mem;
    delete status_lock;
		//delete task_queue; //this should be completely free of tasks by now
		delete task_stack;
};

bool Worker::terminate() {
    ProcStatus master_status;
		status_mem->get(&master_status, MASTER, 1, 0, MPI_INT);
    if(master_status != TERMINATED) return false;
    return true;
};

int Worker::getId() {
	return id;
};

void Worker::set_status(ProcStatus status) {
	status_mem->put(&status, id, 1, 0, MPI_INT);
};

ProcStatus Worker::get_status(int _id) {
		ProcStatus status;
		status_mem->put(&status, _id, 1, 0, MPI_INT);	
		return status;
};

bool Worker::available(int dst_id) {
	//return !task_queue->is_full(dst_id);
	return !task_stack->is_full(dst_id);
};

bool Worker::send_job(int dst_id, _stub *job) { 
	//return task_queue->enqueue(dst_id, job);
	return task_stack->push(dst_id, job);
};

_stub *Worker::get_job() { 
	//return task_queue->dequeue(id);
	return task_stack->pop(id);
};

bool Worker::has_job() {
	//return !task_queue->is_empty(id);
	return !task_stack->is_empty(id);
};


