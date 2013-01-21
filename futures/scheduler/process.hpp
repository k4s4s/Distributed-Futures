
#ifndef _PROCESS_H
#define _PROCESS_H

#include "../future_fwd.hpp"
#include "taskQueue.hpp"
#include "taskStack.hpp"

#define TQUEUE_SIZE 1000

namespace futures {
namespace scheduler {

enum ProcStatus { IDLE=0, RUNNING=1, TERMINATED=2 };

class Process {
public:
    virtual ~Process() {};
		virtual int getId() = 0;
    virtual void set_status(ProcStatus status) = 0;
		virtual ProcStatus get_status(int id) = 0;
    virtual bool terminate() = 0;
		virtual bool available(int dst_id) = 0;
		virtual bool send_job(int dst_id, _stub *job) = 0;
		virtual _stub *get_job() = 0;
		virtual bool has_job() = 0;
};

}
}

#endif
