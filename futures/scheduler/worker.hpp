#ifndef _WORKER_H
#define _WORKER_H

#include "process.hpp"
#include "../communication/communication.hpp"
#include <mutex.hpp>

namespace futures {
namespace scheduler {

class Worker : public Process {
private:
		communication::CommInterface *comm;
		communication::Shared_Address_space *status_mem;
		mutex *status_lock;
		ProcStatus *status;
		//taskQueue *task_queue;
		taskStack *task_stack;
    int id;
public:
    Worker(communication::CommInterface *_comm);
    ~Worker();
		int getId();
    void set_status(ProcStatus status);
		ProcStatus get_status(int _id);
    bool terminate();
		bool available(int dst_id);
		bool send_job(int dst_id, std::shared_ptr<_stub>& job);
		std::shared_ptr<_stub> get_job();
		bool has_job();
};

}
}

#endif
