
#include "RR.hpp"
#include "../communication/communication.hpp"
#include "../communication/mpi_details.hpp"
#include <cassert>
#include "../common.hpp"

#define MASTER_ID 0

using namespace futures::scheduler;
using namespace futures::communication;

RRScheduler::RRScheduler(CommInterface* _comm) {
    comm = _comm;
    total_workers = comm->size();
    master_id = 0;
    int id = comm->get_procId();
		if(id == master_id) {
    	proc = new Master(comm);
			sched_mem = comm->new_shared_space(sizeof(int));
			sched_lock = comm->new_lock();
			int curr_worker_id = master_id;
			sched_mem->put(&curr_worker_id, id, 1, 0, MPI_INT);
		}
    else {
      proc = new Worker(comm);
			//TODO: create an option to not actually allocate space
			sched_mem = comm->new_shared_space(sizeof(int));
			sched_lock = comm->new_lock();
		}
};

RRScheduler::~RRScheduler() {
		int id = comm->get_procId();
		delete sched_mem;
		delete sched_lock;
    delete proc;
};

Scheduler* RRScheduler:: create(CommInterface *commInterface) {
    return new RRScheduler(commInterface);
};

int RRScheduler::nextAvaibleWorkerId() {
    assert(total_workers != 0);
		int curr_worker_id;
		sched_lock->lock(MASTER_ID);
		sched_mem->get(&curr_worker_id, MASTER_ID, 1, 0, MPI_INT);
    curr_worker_id = (curr_worker_id+1)%(total_workers);
		curr_worker_id = (curr_worker_id==0)?curr_worker_id+1:curr_worker_id;
		sched_mem->put(&curr_worker_id, MASTER_ID, 1, 0, MPI_INT);
		//DPRINT_VAR("\tRRSched:next proc in RR is ", curr_worker_id);
		//find the next idle or at least avaible proc
		for(int i = curr_worker_id; i < total_workers; i++) {	
			if(proc->available(i)) {
				sched_lock->unlock(MASTER_ID);
				//DPRINT_VAR("\tRRSched:worker is available, ", curr_worker_id);
				return i;	
			}
		}
		//if we get here we didn't find any avaible procs, so run RR fashion
		//DPRINT_VAR("\tRRSched:No idle proc found, returning ", curr_worker_id);
		sched_lock->unlock(MASTER_ID);
    return curr_worker_id;
};

void RRScheduler::set_status(ProcStatus status) {
    proc->set_status(status);
};

bool RRScheduler::terminate() {
    return proc->terminate();
};

void RRScheduler::schedule_proc() {
	int id;
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
	while(proc->has_job()) {
		//DPRINT_VAR("RRScheduler:worker has job! ", id);
		//DPRINT_VAR("\tRRScheduler:setting status to running ", id);
		proc->set_status(scheduler::ProcStatus::RUNNING);
		//DPRINT_VAR("\tRRScheduler:receiving job ", id);
		_stub *job = proc->get_job();
		//DPRINT_VAR("\tRRScheduler:running job! ", id);
		job->run();
		proc->set_status(scheduler::ProcStatus::IDLE);
	}
};

bool RRScheduler::schedule_job(int dst_id, _stub *job) {
	return proc->send_job(dst_id, job);
};




