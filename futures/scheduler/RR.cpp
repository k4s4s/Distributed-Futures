
#include "RR.hpp"
#include "../communication/communication.hpp"
#include "../communication/mpi_details.hpp"
#include <cassert>
#include "../common.hpp"

using namespace futures::scheduler;
using namespace futures::communication;

RRScheduler::RRScheduler(CommInterface* _comm) {
    comm = _comm;
    total_workers = comm->size();
    master_id = 0;
    int id = comm->get_procId();
		if(id == master_id) {
    	proc = new Master();
			MPI_Alloc_mem(sizeof(int), MPI_INFO_NULL, &sched_mem);
  		MPI_Win_create(sched_mem, sizeof(int), 1, MPI_INFO_NULL, 
										MPI_COMM_WORLD, &sched_win);
			sched_lock = new MPIMutex(MPI_COMM_WORLD);
			int curr_worker_id = master_id;
	 		communication::details::lock_and_put((void*)&curr_worker_id, 1, MPI_INT, id, 
																					0, 1, MPI_INT, sched_win, sched_lock);
		}
    else {
      proc = new Worker();
  		MPI_Win_create(NULL, sizeof(int), 1, MPI_INFO_NULL, 
										MPI_COMM_WORLD, &sched_win);
			sched_lock = new MPIMutex(MPI_COMM_WORLD);
		}
};

RRScheduler::~RRScheduler() {
		int id;
		MPI_Comm_rank(MPI_COMM_WORLD, &id);
		MPI_Win_free(&sched_win);
		if(id == 0) {
  		MPI_Free_mem(sched_mem);
		}
		delete sched_lock;
    delete proc;
};

Scheduler* RRScheduler:: create(CommInterface *commInterface) {
    return new RRScheduler(commInterface);
};

int RRScheduler::nextAvaibleWorkerId() {
    assert(total_workers != 0);
		int curr_worker_id;
	 	communication::details::lock_and_get((void*)&curr_worker_id, 1, MPI_INT, 0, 
													0, 1, MPI_INT, sched_win, sched_lock);
    curr_worker_id = (curr_worker_id+1)%(total_workers);
		curr_worker_id = (curr_worker_id==0)?curr_worker_id+1:curr_worker_id;
	 	communication::details::lock_and_put((void*)&curr_worker_id, 1, MPI_INT, 0, 
													0, 1, MPI_INT, sched_win, sched_lock);
		DPRINT_VAR("\tRRSched:next proc in RR is ", curr_worker_id);
		//find the next idle or at least avaible proc
		for(int i = curr_worker_id; i < total_workers; i++) {	
			if(proc->available(i)) {
				DPRINT_VAR("\tRRSched: worker is available", curr_worker_id);
				return i;	
			}
		}
		//if we get here we didn't find any avaible procs, so run RR fashion
		DPRINT_VAR("\tRRSched: No idle proc found, returning ", curr_worker_id);
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
	while(!proc->terminate()) {
		if(proc->has_job()) {
			DPRINT_MESSAGE("\tRRScheduler:worker has job!");
			proc->set_status(scheduler::ProcStatus::RUNNING);
			_stub *job = proc->get_job();
			job->run();
			proc->set_status(scheduler::ProcStatus::IDLE);
		}
	}
};

bool RRScheduler::schedule_job(int dst_id, _stub *job) {
	return proc->send_job(dst_id, job);
};




