
#include "master.hpp"
#include "../communication/mpi_details.hpp"
#include "../common.hpp"

#define MASTER 0

using namespace futures;
using namespace scheduler;

Master::Master() {
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &id);
    MPI_Comm_size(comm, &nprocs);
    MPI_Alloc_mem(nprocs*sizeof(ProcStatus), MPI_INFO_NULL, &status_vector);
    status_vector[0] = RUNNING;
    MPI_Win_create(status_vector, nprocs, sizeof(ProcStatus), MPI_INFO_NULL, comm, &status_win);
    status_lock = new MPIMutex(comm);
};

Master::~Master() {
    MPI_Win_free(&status_win);
    delete status_lock;
};


void Master::set_status(ProcStatus status) {
    communication::details::lock_and_put(status_vector, status, MPI_INT, MASTER, 0, 1,
                                         MPI_INT, status_win, status_lock);
};

int Master::getId() {
	return id;
};

ProcStatus Master::get_status(int _id) {
		ProcStatus status;
		DPRINT_VAR("Master::Trying to status of ", _id);
    communication::details::lock_and_get(&status, 1, MPI_INT, MASTER, id, 1,
                                         MPI_INT, status_win, status_lock);		
	  DPRINT_VAR("Master:", status);
		return status;
};

bool Master::terminate() {
    ProcStatus worker_status;
    for(int i=1; i < nprocs; i++) {
        communication::details::lock_and_get(&worker_status, 1, MPI_INT, MASTER, i, 1,
                                             MPI_INT, status_win, status_lock);
        if(!worker_status == RUNNING) break;
        else if(i == nprocs-1) {
            ProcStatus status = RUNNING;
            communication::details::lock_and_put(&status, 1, MPI_INT, MASTER, 0, 1,
                                                 MPI_INT, status_win, status_lock);
            return true;
        }
    }
    return false;
};

