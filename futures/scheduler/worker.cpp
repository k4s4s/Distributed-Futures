
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
    MPI_Win_create(NULL, nprocs, sizeof(ProcStatus), MPI_INFO_NULL, comm, &status_win);
    status_lock = new MPIMutex(comm);
    this->set_status(IDLE);
};

Worker::~Worker() {
    MPI_Win_free(&status_win);
    delete status_lock;
};

bool Worker::terminate() {
    ProcStatus master_status;
    communication::details::lock_and_get(&master_status, 1, MPI_INT, MASTER, 0, 1,
                                         MPI_INT, status_win, status_lock);
    if(master_status != TERMINATED) return true;
    return false;
};

int Worker::getId() {
	return id;
};

void Worker::set_status(ProcStatus status) {
    communication::details::lock_and_put(&status, 1, MPI_INT, MASTER, id, 1,
                                         MPI_INT, status_win, status_lock);
};

ProcStatus Worker::get_status(int _id) {
		ProcStatus status;
    communication::details::lock_and_get(&status, 1, MPI_INT, MASTER, id, 1,
                                         MPI_INT, status_win, status_lock);		
		return status;
};
