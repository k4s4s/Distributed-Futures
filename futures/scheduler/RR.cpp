
#include "RR.hpp"
#include "../communication/communication.hpp"
#include <cassert>
#include "../common.hpp"

using namespace futures::scheduler;
using namespace futures::communication;

RRScheduler::RRScheduler(CommInterface* _comm) {
    comm = _comm;
    total_workers = comm->size();
    master_id = 0;
    int id = comm->get_procId();
    curr_worker_id = id;
    if(id == master_id)
        proc = new Master();
    else
        proc = new Worker();
};

RRScheduler::~RRScheduler() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    delete proc;
};

Scheduler* RRScheduler:: create(CommInterface *commInterface) {
    return new RRScheduler(commInterface);
};

int RRScheduler::nextAvaibleWorkerId() {
    assert(total_workers != 0);
    curr_worker_id = (curr_worker_id+1)%(total_workers);
		curr_worker_id = (curr_worker_id==0)?curr_worker_id+1:curr_worker_id;
		DPRINT_VAR("RRSched:", curr_worker_id);
		for(int i = curr_worker_id; i < total_workers; i++) {
				DPRINT_VAR("RRSched:", proc->get_status(i));
				DPRINT_VAR("RRSched:", i);	
			if(proc->get_status(i) == IDLE) {
				curr_worker_id = i;			
				DPRINT_VAR("RRSched:", curr_worker_id);
				return curr_worker_id;
			}
		}
		//if we get here we didn't find any idle procs, so run on self
		curr_worker_id = proc->getId();
		DPRINT_VAR("RRSched:", curr_worker_id);
    return curr_worker_id;
};

void RRScheduler::set_status(ProcStatus status) {
    proc->set_status(status);
};

bool RRScheduler::terminate() {
    return proc->terminate();
};

