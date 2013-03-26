
#ifndef _TASKSTACK_H
#define _TASKSTACK_H

#include <mutex.hpp>
#include "../future_fwd.hpp"
#include "../communication/communication.hpp"

#define MAX_STACK_SIZE 10000 //waste of space though...and a bug waiting to happen also!
#define TASKSTACK_SIZE 4*1024*1024*8 //4MB
#define STACK_START_OFFSET 2*sizeof(int)
#define SIZE_OFFSET 0
#define HEAD_OFFSET sizeof(int)
#define TASK_OFFSET sizeof(int)

namespace futures {
namespace scheduler {

class taskStack {
private:
	//stack head, tail and size are allocated in this space
	communication::CommInterface *comm;
	communication::Shared_Address_space *taskS; 
	mutex *taskS_lock;
public:
	taskStack(communication::CommInterface *_comm);
	~taskStack();
	bool push(int dst_id, _stub *job);
	_stub *pop(int dst_id);
	bool is_empty(int dst_id);
	bool is_full(int dst_id);
};

}
}

#endif
