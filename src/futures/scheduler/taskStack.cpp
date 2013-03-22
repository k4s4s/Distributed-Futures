
#include "taskStack.hpp"
#include "../common.hpp"

using namespace futures;
using namespace scheduler;

taskStack::taskStack(communication::CommInterface *_comm) {
	comm = _comm;
	taskS = comm->new_shared_space(TASKSTACK_SIZE);
	taskS_lock = comm->new_lock();
	int id = comm->get_procId();
	int curr_head, curr_size;
	curr_head = STACK_START_OFFSET;
	curr_size = 0;
	taskS->put(curr_size, id, 1, SIZE_OFFSET);
	taskS->put(curr_head, id, 1, HEAD_OFFSET);
};

taskStack::~taskStack() {
	delete taskS;
	delete taskS_lock;
};

bool taskStack::push(int dst_id, _stub *job) {
	//DPRINT_VAR("\t\ttaskstack:Pushing job to ", dst_id);
	int curr_size, curr_head;
  _stub_wrapper tw(job);
	taskS_lock->lock(dst_id);
	/*read list tail*/
	curr_head = taskS->get<int>(dst_id, 1, HEAD_OFFSET);
	curr_size = taskS->get<int>(dst_id, 1, SIZE_OFFSET);
	if(curr_size >= MAX_STACK_SIZE) return false;
	/* add new task to list */
	
	DPRINT_VAR("\t\ttaskstack:Push:inserting at ", curr_head);
	int task_size = taskS->put(tw, dst_id, 1, curr_head);
	DPRINT_VAR("\t\ttaskstack:Push:till ", curr_head+TASK_OFFSET+task_size);
	DPRINT_VAR("\t\ttaskstack:Push:", task_size);
	taskS->put(task_size, dst_id, 1, curr_head+task_size);
	/* set new list head */
	curr_head += TASK_OFFSET+task_size;
	curr_size++;
	taskS->put(curr_head, dst_id, 1, HEAD_OFFSET);
	taskS->put(curr_size, dst_id, 1, SIZE_OFFSET);
	taskS_lock->unlock(dst_id);
	return true;
};

_stub *taskStack::pop(int dst_id) {
	//DPRINT_VAR("\t\ttaskstack:Pop job from ", dst_id);
	int curr_head, curr_size;
	taskS_lock->lock(dst_id);
	/*read list head*/
	curr_head = taskS->get<int>(dst_id, 1, HEAD_OFFSET);
	curr_size = taskS->get<int>(dst_id, 1, SIZE_OFFSET);
	if(curr_size == 0)
		return NULL;
	/* get next task */
	int task_size = taskS->get<int>(dst_id, 1, curr_head-TASK_OFFSET);
  // Prepare input buffer and receive the message
	
	DPRINT_VAR("\t\ttaskstack:Pop:actual head ", curr_head);
	DPRINT_VAR("\t\ttaskstack:Pop:poping from ", curr_head-task_size-TASK_OFFSET);
	DPRINT_VAR("\t\ttaskstack:Pop:", task_size);
	
	_stub_wrapper tw = taskS->get<_stub_wrapper>(dst_id, task_size, 
																							curr_head-task_size-TASK_OFFSET);
	DPRINT_MESSAGE("\t\ttaskstack:got data");
	DPRINT_MESSAGE("\t\ttaskstack:created empty _stub_wrapper");
	DPRINT_MESSAGE("\t\ttaskstack:streamed data");
	/* set new list head */
	curr_head -= TASK_OFFSET+task_size;
	DPRINT_VAR("\t\ttaskstack:Pop:next head ", curr_head);
	curr_size--;
	taskS->put(curr_head, dst_id, 1, HEAD_OFFSET);
	taskS->put(curr_size, dst_id, 1, SIZE_OFFSET);
	taskS_lock->unlock(dst_id);
	DPRINT_MESSAGE("\t\ttaskstack:returning task");
	return tw.get_task();
};

bool taskStack::is_empty(int dst_id) {
	int curr_size;
	taskS_lock->lock(dst_id);
	curr_size = taskS->get<int>(dst_id, 1, SIZE_OFFSET);
	taskS_lock->unlock(dst_id);
	if(curr_size == 0) return true;
	return false;
};
 
bool taskStack::is_full(int dst_id) {
	int curr_size;
	taskS_lock->lock(dst_id);
	curr_size = taskS->get<int>(dst_id, 1, SIZE_OFFSET);
	taskS_lock->unlock(dst_id);
	if(curr_size == MAX_STACK_SIZE) return true;
	return false;
};
