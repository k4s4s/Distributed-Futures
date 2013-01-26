
#ifndef _TASKSTACK_H
#define _TASKSTACK_H

#include <mpi.h>
#include <MPIMutex.hpp>
#include "../future_fwd.hpp"

#define MAX_STACK_SIZE 1000 //waste of space though...and a bug waiting to happen also!
#define TASKSTACK_SIZE 4*1024*1024*8 //4MB
#define STACK_START_OFFSET 2*sizeof(int)
#define SIZE_OFFSET 0
#define HEAD_OFFSET sizeof(int)
#define TASK_OFFSET sizeof(int)

namespace futures {
namespace scheduler {

class taskStack {
private:
	MPI_Win taskS_win;
	void *taskS; //stack head, tail and size are allocated in this space
	MPIMutex *taskS_lock;
public:
	taskStack();
	~taskStack();
	bool push(int dst_id, _stub *job);
	_stub *pop(int dst_id);
	bool is_empty(int dst_id);
	bool is_full(int dst_id);
};

}
}

#endif
