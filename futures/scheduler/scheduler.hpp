#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <memory>
#include "../communication/communication.hpp"
#include "process.hpp"

namespace futures {
namespace scheduler {

class Scheduler {
public:
    virtual ~Scheduler() {};
    virtual int nextAvaibleWorkerId() = 0;
    virtual void set_status(ProcStatus status) = 0;
    virtual bool terminate() = 0;
		virtual void run_proc() = 0;
		virtual bool schedule_job(int dst_id, std::shared_ptr<_stub>& job) = 0;
};

}
}
#endif
