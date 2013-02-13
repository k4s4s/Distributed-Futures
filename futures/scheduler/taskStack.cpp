
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
	taskS->put(&curr_size, id, 1, SIZE_OFFSET, MPI_INT);
	taskS->put(&curr_head, id, 1, HEAD_OFFSET, MPI_INT);
};

taskStack::~taskStack() {
	delete taskS;
	delete taskS_lock;
};

bool taskStack::push(int dst_id, _stub *job) {
	//DPRINT_VAR("\t\ttaskstack:Pushing job to ", dst_id);
	int curr_size, curr_head;
  boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
  _stub_wrapper tw(job);
  oa << tw;
	taskS_lock->lock(dst_id);
	/*read list tail*/
	taskS->get(&curr_head, dst_id, 1, HEAD_OFFSET, MPI_INT);
	taskS->get(&curr_size, dst_id, 1, SIZE_OFFSET, MPI_INT);
	if(curr_size >= MAX_STACK_SIZE) return false;
	/* add new task to list */
	/*
	DPRINT_VAR("\t\ttaskstack:Push:inserting at ", curr_head);
	DPRINT_VAR("\t\ttaskstack:Push:till", curr_head+oa.size());
	DPRINT_VAR("\t\ttaskstack:Push:count ", oa.size());
	*/
	taskS->put((void*)(&oa.size()), dst_id, 1, curr_head+oa.size(), MPI_INT);
	taskS->put(const_cast<void*>(oa.address()), dst_id, oa.size(), curr_head, MPI_PACKED);
	/* set new list head */
	curr_head += TASK_OFFSET+oa.size();
	curr_size++;
	taskS->put(&curr_head, dst_id, 1, HEAD_OFFSET, MPI_INT);
	taskS->put(&curr_size, dst_id, 1, SIZE_OFFSET, MPI_INT);
	taskS_lock->unlock(dst_id);
	return true;
};

_stub *taskStack::pop(int dst_id) {
	//DPRINT_VAR("\t\ttaskstack:Pop job from ", dst_id);
	int curr_head, curr_size;
  boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
	taskS_lock->lock(dst_id);
	/*read list head*/
	taskS->get(&curr_head, dst_id, 1, HEAD_OFFSET, MPI_INT);
	taskS->get(&curr_size, dst_id, 1, SIZE_OFFSET, MPI_INT);
	if(curr_size == 0)
		return NULL;
	/* get next task */
  int count;
	taskS->get(&count, dst_id, 1, curr_head-TASK_OFFSET, MPI_INT);
  // Prepare input buffer and receive the message
	/*
	DPRINT_VAR("\t\ttaskstack:Pop:actual head ", curr_head);
	DPRINT_VAR("\t\ttaskstack:Pop:poping from ", curr_head-count-TASK_OFFSET);
	DPRINT_VAR("\t\ttaskstack:Pop:count ", count);  
	*/
	ia.resize(count);
	taskS->get(const_cast<void*>(ia.address()), dst_id, ia.size(), 
						curr_head-count-TASK_OFFSET, MPI_PACKED);
  _stub_wrapper tw;
  ia >> tw;
	/* set new list head */
	curr_head -= TASK_OFFSET+count;
	//DPRINT_VAR("\t\ttaskstack:Pop:next head ", curr_head-count-TASK_OFFSET);
	curr_size--;
	taskS->put(&curr_head, dst_id, 1, HEAD_OFFSET, MPI_INT);
	taskS->put(&curr_size, dst_id, 1, SIZE_OFFSET, MPI_INT);
	taskS_lock->unlock(dst_id);
	return tw.get_task();
};

bool taskStack::is_empty(int dst_id) {
	int curr_size;
	taskS_lock->lock(dst_id);
	taskS->get(&curr_size, dst_id, 1, SIZE_OFFSET, MPI_INT);
	taskS_lock->unlock(dst_id);
	if(curr_size == 0) return true;
	return false;
};
 
bool taskStack::is_full(int dst_id) {
	int curr_size;
	taskS_lock->lock(dst_id);
	taskS->get(&curr_size, dst_id, 1, SIZE_OFFSET, MPI_INT);
	taskS_lock->unlock(dst_id);
	if(curr_size == MAX_STACK_SIZE) return true;
	return false;
};

