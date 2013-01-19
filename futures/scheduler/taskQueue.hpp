
#ifndef _TASKQUEUE_H
#define _TASKQUEUE_H

#include <mpi.h>
#include <MPIMutex.hpp>
#include "../future_fwd.hpp"

#define MAX_QUEUE_SIZE 1000
#define TASKQUEUE_SIZE MAX_QUEUE_SIZE*(sizeof(int)*sizeof(_stub_wrapper))+3*sizeof(int)
#define QUEUE_START_OFFSET 3*sizeof(int)
#define SIZE_OFFSET 0
#define HEAD_OFFSET sizeof(int)
#define TAIL_OFFSET 2*sizeof(int)
#define TASK_OFFSET sizeof(int)

namespace futures {
namespace scheduler {

class taskQueue {
private:
	MPI_Win taskQ_win;
	void *taskQ; //queue head, tail and size are allocated in this space
	MPIMutex *taskQ_lock;
public:
	taskQueue();
	~taskQueue();
	bool enqueue(int dst_id, _stub *job);
	_stub *dequeue(int dst_id);
	bool is_empty(int dst_id);
	bool is_full(int dst_id);
};

}
}

#endif
