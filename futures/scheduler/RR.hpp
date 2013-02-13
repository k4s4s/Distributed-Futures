#ifndef _RR_H
#define _RR_H

#include "scheduler.hpp"
#include "../communication/communication.hpp"
#include <map>
#include "taskQueue.hpp"
#include <mutex.hpp>

namespace futures {
namespace scheduler {

class RRScheduler : public Scheduler {
private:
    unsigned int master_id;
    unsigned int total_workers;
    communication::Shared_Address_space *sched_mem;
		mutex *sched_lock;
    communication::CommInterface* comm;
    Process *proc;
public:
    RRScheduler(communication::CommInterface* commInterface);
    ~RRScheduler();
    static Scheduler* create(communication::CommInterface *commInterface);
    int nextAvaibleWorkerId();
    void set_status(ProcStatus status);
    bool terminate();
		void schedule_proc();
		bool schedule_job(int dst_id, _stub *job);
};

}
}

#endif
