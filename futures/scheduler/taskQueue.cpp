/*TODO: Make queue cyclic, so we won't waste space*/
#include "taskQueue.hpp"
#include "../communication/mpi_details.hpp"
#include "../common.hpp"

using namespace futures;
using namespace scheduler;

taskQueue::taskQueue() {
	MPI_Alloc_mem(TASKQUEUE_SIZE, MPI_INFO_NULL, &taskQ);
  MPI_Win_create(taskQ, TASKQUEUE_SIZE, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &taskQ_win);
	taskQ_lock = new MPIMutex(MPI_COMM_WORLD);
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	int curr_head, curr_tail, curr_size;
	curr_head = curr_tail = QUEUE_START_OFFSET;
	curr_size = 0;
  communication::details::lock_and_put((void*)&curr_size, 1, MPI_INT, id, SIZE_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);
  communication::details::lock_and_put((void*)&curr_head, 1, MPI_INT, id, HEAD_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);	
  communication::details::lock_and_put((void*)&curr_tail, 1, MPI_INT, id, TAIL_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);
};

taskQueue::~taskQueue() {
	MPI_Win_free(&taskQ_win);
  MPI_Free_mem(taskQ);
	delete taskQ_lock;
};

bool taskQueue::enqueue(int dst_id, _stub *job) {
	DPRINT_VAR("\t\ttaskQueue:Enqueuing job to ", dst_id);
	int curr_tail, curr_size;
  boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
  _stub_wrapper tw(job);
  oa << tw;
	taskQ_lock->lock(dst_id);
	/*read list tail*/
  communication::details::lock_and_get((void*)&curr_tail, 1, MPI_INT, dst_id, TAIL_OFFSET,
                        							1, MPI_INT, taskQ_win, NULL);
  communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskQ_win, NULL);
	if(curr_size >= MAX_QUEUE_SIZE) return false;
	/* add new task to list */
  communication::details::lock_and_put((void*)(&oa.size()), 1, MPI_INT, dst_id, curr_tail,
                          1, MPI_INT, taskQ_win, NULL);
  communication::details::lock_and_put(const_cast<void*>(oa.address()), oa.size(), MPI_PACKED, dst_id, 
													curr_tail+TASK_OFFSET, oa.size(), MPI_PACKED, taskQ_win, NULL);
	/* set new list tail */
	curr_tail += sizeof(int)+sizeof(_stub_wrapper);
	curr_size++;
  communication::details::lock_and_put((void*)&curr_tail, 1, MPI_INT, dst_id, TAIL_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);
  communication::details::lock_and_put((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);
	taskQ_lock->unlock(dst_id);
	DPRINT_VAR("\t\ttaskQueue:Enqueue:tail ", curr_tail);
	DPRINT_VAR("\t\ttaskQueue:Enqueue:count ", oa.size());
	return true;
};

_stub *taskQueue::dequeue(int dst_id) {
	//DPRINT_VAR("\t\ttaskQueue:Dequeuing job from ", dst_id);
	int curr_head, curr_size;
  boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
	taskQ_lock->lock(dst_id);
	/*read list head*/
	communication::details::lock_and_get((void*)&curr_head, 1, MPI_INT, dst_id, HEAD_OFFSET,
                        							1, MPI_INT, taskQ_win, NULL);
  communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskQ_win, NULL);
	if(curr_size == 0)
		return NULL;
	/* get next task */
  int count;
  communication::details::lock_and_get((void*)&count, 1, MPI_INT, dst_id, curr_head,
                          						1, MPI_INT, taskQ_win, NULL);
  // Prepare input buffer and receive the message
	/*
	DPRINT_VAR("\t\ttaskQueue:Dequeue:head ", curr_head);
	DPRINT_VAR("\t\ttaskQueue:Dequeue:size ", curr_size);
	DPRINT_VAR("\t\ttaskQueue:Dequeue:count ", count);  
	*/
	ia.resize(count);
  communication::details::lock_and_get(const_cast<void*>(ia.address()), ia.size(), MPI_PACKED, dst_id,
                          curr_head+TASK_OFFSET, ia.size(), MPI_PACKED, taskQ_win, NULL);
  _stub_wrapper tw;
  ia >> tw;
	/* set new list head */
	curr_head += sizeof(int)+sizeof(_stub_wrapper);
	curr_size--;
  communication::details::lock_and_put((void*)&curr_head, 1, MPI_INT, dst_id, HEAD_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);
  communication::details::lock_and_put((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                          						1, MPI_INT, taskQ_win, NULL);
	taskQ_lock->unlock(dst_id);
	return tw.get_task();
};

bool taskQueue::is_empty(int dst_id) {
	int curr_size;
	taskQ_lock->lock(dst_id);
	communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskQ_win, NULL);
	taskQ_lock->unlock(dst_id);
	if(curr_size == 0) return true;
	return false;
};
 
bool taskQueue::is_full(int dst_id) {
	int curr_size;
	taskQ_lock->lock(dst_id);
	communication::details::lock_and_get((void*)&curr_size, 1, MPI_INT, dst_id, SIZE_OFFSET,
                        							1, MPI_INT, taskQ_win, NULL);
	taskQ_lock->unlock(dst_id);
	if(curr_size == MAX_QUEUE_SIZE) return true;
	return false;
};


