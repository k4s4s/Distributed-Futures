
#include "taskStack.hpp"
#include "../communication/mpi_details.hpp"
#include "../common.hpp"

using namespace futures;
using namespace scheduler;

taskStack::taskStack() {
	MPI_Alloc_mem(TASKSTACK_SIZE, MPI_INFO_NULL, &taskS);
  MPI_Win_create(taskS, TASKSTACK_SIZE, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &taskS_win);
	taskS_lock = new MPIMutex(MPI_COMM_WORLD);
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	int curr_head, curr_size;
	curr_head = STACK_START_OFFSET;
	curr_size = 0;
  communication::details::lock_and_put((void*)&curr_size, 1, MPI_INT, id, SIZE_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);
  communication::details::lock_and_put((void*)&curr_head, 1, MPI_INT, id, HEAD_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);	
};

taskStack::~taskStack() {
	MPI_Win_free(&taskS_win);
  MPI_Free_mem(taskS);
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
  communication::details::lock_and_get((void*)&curr_head, 1, MPI_INT, dst_id, HEAD_OFFSET,
                        							1, MPI_INT, taskS_win, NULL);
  communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskS_win, NULL);
	if(curr_size >= MAX_STACK_SIZE) return false;
	/* add new task to list */
	/*
	DPRINT_VAR("\t\ttaskstack:Push:inserting at ", curr_head);
	DPRINT_VAR("\t\ttaskstack:Push:till", curr_head+oa.size());
	DPRINT_VAR("\t\ttaskstack:Push:count ", oa.size());
	*/
  communication::details::lock_and_put((void*)(&oa.size()), 1, MPI_INT, dst_id, curr_head+oa.size(),
                          1, MPI_INT, taskS_win, NULL);
  communication::details::lock_and_put(const_cast<void*>(oa.address()), oa.size(), MPI_PACKED, dst_id, 
													curr_head, oa.size(), MPI_PACKED, taskS_win, NULL);
	/* set new list head */
	curr_head += TASK_OFFSET+oa.size();
	curr_size++;
  communication::details::lock_and_put((void*)&curr_head, 1, MPI_INT, dst_id, HEAD_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);
  communication::details::lock_and_put((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);
	taskS_lock->unlock(dst_id);
	return true;
};

_stub *taskStack::pop(int dst_id) {
	//DPRINT_VAR("\t\ttaskstack:Pop job from ", dst_id);
	int curr_head, curr_size;
  boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
	taskS_lock->lock(dst_id);
	/*read list head*/
	communication::details::lock_and_get((void*)&curr_head, 1, MPI_INT, dst_id, HEAD_OFFSET,
                        							1, MPI_INT, taskS_win, NULL);
  communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskS_win, NULL);
	if(curr_size == 0)
		return NULL;
	/* get next task */
  int count;
  communication::details::lock_and_get((void*)&count, 1, MPI_INT, dst_id, curr_head-TASK_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);
  // Prepare input buffer and receive the message
	/*
	DPRINT_VAR("\t\ttaskstack:Pop:actual head ", curr_head);
	DPRINT_VAR("\t\ttaskstack:Pop:poping from ", curr_head-count-TASK_OFFSET);
	DPRINT_VAR("\t\ttaskstack:Pop:count ", count);  
	*/
	ia.resize(count);
  communication::details::lock_and_get(const_cast<void*>(ia.address()), ia.size(), MPI_PACKED, dst_id,
                          curr_head-count-TASK_OFFSET, ia.size(), MPI_PACKED, taskS_win, NULL);
  _stub_wrapper tw;
  ia >> tw;
	/* set new list head */
	curr_head -= TASK_OFFSET+count;
	//DPRINT_VAR("\t\ttaskstack:Pop:next head ", curr_head-count-TASK_OFFSET);
	curr_size--;
  communication::details::lock_and_put((void*)&curr_head, 1, MPI_INT, dst_id, HEAD_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);
  communication::details::lock_and_put((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                          						1, MPI_INT, taskS_win, NULL);
	taskS_lock->unlock(dst_id);
	return tw.get_task();
};

bool taskStack::is_empty(int dst_id) {
	int curr_size;
	taskS_lock->lock(dst_id);
	communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskS_win, NULL);
	taskS_lock->unlock(dst_id);
	if(curr_size == 0) return true;
	return false;
};
 
bool taskStack::is_full(int dst_id) {
	int curr_size;
	taskS_lock->lock(dst_id);
	communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskS_win, NULL);
	taskS_lock->unlock(dst_id);
	if(curr_size == MAX_STACK_SIZE) return true;
	return false;
};


