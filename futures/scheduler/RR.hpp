#ifndef _RR_H
#define _RR_H

#include "scheduler.hpp"
#include "../communication/communication.hpp"

namespace futures {
namespace scheduler {

class RRScheduler : public Scheduler {
private:
	unsigned int master_id;
	unsigned int total_workers;
	unsigned int curr_worker_id;
	communication::CommInterface* comm;
public:
	RRScheduler(communication::CommInterface* commInterface);
	~RRScheduler();
	static Scheduler* create(communication::CommInterface *commInterface);
	int nextAvaibleWorkerId();
};

}
}

#endif
