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
	curr_worker_id = 0;
	int id = comm->get_procId();
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
	curr_worker_id = ((curr_worker_id+1)%(total_workers-1))+1;
	return curr_worker_id;
};

void RRScheduler::set_status(ProcStatus status) {
	proc->set_status(status);
};

bool RRScheduler::terminate() {
	return proc->terminate();
};

