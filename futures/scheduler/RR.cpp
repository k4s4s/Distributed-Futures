#include "RR.hpp"
#include "../communication/communication.hpp"

using namespace futures::scheduler;
using namespace futures::communication;

RRScheduler::RRScheduler(CommInterface* _comm) {
	comm = _comm;
	total_workers = comm->size();
	master_id = 0;
	curr_worker_id = (master_id+1)%total_workers;
};

RRScheduler::~RRScheduler() {

};

Scheduler* RRScheduler:: create(CommInterface *commInterface) {
	return new RRScheduler(commInterface);
};

int RRScheduler::nextAvaibleWorkerId() {
	curr_worker_id = (curr_worker_id+1)%total_workers;
};
