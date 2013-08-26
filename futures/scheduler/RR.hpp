#ifndef _RR_H
#define _RR_H

#include <map>
#include <mutex.hpp>

#include "../communication/communication.hpp"
#include "scheduler.hpp"

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
		void run_proc();
		bool schedule_job(int dst_id, std::shared_ptr<_stub>& job);
};

}
}

#endif
