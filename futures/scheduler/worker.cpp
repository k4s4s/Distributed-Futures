
#include "worker.hpp"
#include "../communication/mpi_details.hpp"
#include "../common.hpp"

#define MASTER 0

using namespace futures;
using namespace scheduler;

Worker::Worker() {
    int nprocs;
    id = details::mutex_count++;
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &id);
    MPI_Comm_size(comm, &nprocs);
    MPI_Alloc_mem(sizeof(ProcStatus), MPI_INFO_NULL, &status);
    MPI_Win_create(status, 1, sizeof(ProcStatus), MPI_INFO_NULL, comm, &status_win);
    status_lock = new MPIMutex(comm);
		task_queue = new taskQueue();
    this->set_status(IDLE);
};

Worker::~Worker() {
    MPI_Win_free(&status_win);
		MPI_Free_mem(status);
    delete status_lock;
		delete task_queue; //this should be completely free of tasks by now
};

bool Worker::terminate() {
    ProcStatus master_status;
    communication::details::lock_and_get(&master_status, 1, MPI_INT, MASTER, 0, 1,
                                         MPI_INT, status_win, status_lock);
    if(master_status != TERMINATED) return false;
    return true;
};

int Worker::getId() {
	return id;
};

void Worker::set_status(ProcStatus status) {
    communication::details::lock_and_put(&status, 1, MPI_INT, id, 0, 1,
                                         MPI_INT, status_win, status_lock);
};

ProcStatus Worker::get_status(int _id) {
		ProcStatus status;
    communication::details::lock_and_get(&status, 1, MPI_INT, _id, 0, 1,
                                         MPI_INT, status_win, status_lock);		
		return status;
};

bool Worker::available(int dst_id) {
	return !task_queue->is_full(dst_id);
};

bool Worker::send_job(int dst_id, _stub *job) { 
	return task_queue->enqueue(dst_id, job);
};

_stub *Worker::get_job() { 
	return task_queue->dequeue(id);
};


bool Worker::has_job() {
	return !task_queue->is_empty(id);
};


