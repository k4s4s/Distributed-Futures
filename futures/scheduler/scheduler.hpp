#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "../communication/communication.hpp"
#include "process.hpp"
#include "master.hpp"
#include "worker.hpp"

namespace futures {
namespace scheduler {

class Scheduler {
public:
    virtual ~Scheduler() {};
    virtual int nextAvaibleWorkerId() = 0;
    virtual void set_status(ProcStatus status) = 0;
    virtual bool terminate() = 0;
};

}
}
#endif
