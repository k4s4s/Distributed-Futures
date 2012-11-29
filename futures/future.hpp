
#ifndef Future_H
#define Future_H

#include <iostream>
#include <cassert>
#include "futures_enviroment.hpp"
#include "communication/communication.hpp"
#include "details.hpp"
#include "future_fwd.hpp"
#include "common.hpp"

namespace futures {

template <class T>
class Future {
private:
    bool ready_status;
    int src_id, dst_id;
		communication::SharedDataManager *sharedData;
public:
		Future(int _src_id, int _dst_id, 
					communication::SharedDataManager *_sharedData);
    ~Future();
    bool is_ready();
    T get();
};

typedef communication::SharedDataManager* TaskHandler;

template<typename T>
TaskHandler init_async_task(int future_owner) {
	DPRINT_MESSAGE("JOB:initilizing job");
  Futures_Enviroment *env = Futures_Enviroment::Instance();
	//get ids, data_size, type_size
	DPRINT_MESSAGE("JOB:receiving job data");
	int data_size = env->recv_data<int>(future_owner);
	int type_size = env->recv_data<int>(future_owner);
	int id = env->get_procId();
	//create shared data on both ends, initial comm is over global communicator(?)
DPRINT_VAR("JOB:", data_size);
DPRINT_VAR("JOB:", type_size);
	DPRINT_MESSAGE("JOB:creating shared data");
	communication::SharedDataManager *sharedData;
	sharedData = env->new_SharedDataManager(id, future_owner, data_size, type_size,
																					details::_get_mpi_datatype<T>()(
																						details::_is_mpi_datatype<T>()));
	return sharedData;
};

template<typename T>
void return_future(T& retVal, TaskHandler taskHandler) {
	//set future;
    details::_set_data<T>()(taskHandler, retVal, details::_is_mpi_datatype<T>());
		int ready_status = 1;
    taskHandler->set_status(&ready_status);
};

/** Implementation of async function **/
template<typename T>
Future<T> *async_impl(unsigned int data_size, AsyncTask& f) {
		DPRINT_MESSAGE("ASYNC:call to async");
    Futures_Enviroment *env = Futures_Enviroment::Instance();
		int type_size = details::_sizeof<T>()(details::_is_mpi_datatype<T>());
    int id = env->get_procId();
		//get worker id and wake him
		DPRINT_MESSAGE("ASYNC:issuing job");
		int worker_id = env->get_avaibleWorker(&f); //this call also wakes worker
		//send function object, worker runs it, get to init phase
		//env->send_job(worker_id, f);
		//send ids, data_size, type_size
		//env->send_job_data(worker_id, id);
		DPRINT_MESSAGE("ASYNC:sending job data");
		DPRINT_VAR("ASYNC:", data_size);
		DPRINT_VAR("ASYNC:", type_size);
		env->send_data(worker_id, data_size);
		env->send_data(worker_id, type_size);
		//create shared data on both ends, initial comm is over global communicator(?)
		DPRINT_MESSAGE("ASYNC:creating shared data");
		communication::SharedDataManager *sharedData;
		sharedData = env->new_SharedDataManager(worker_id, id, data_size, type_size,
																						details::_get_mpi_datatype<T>()(
																							details::_is_mpi_datatype<T>()));
		//return future
		DPRINT_MESSAGE("ASYNC:returning future");
		Future<T> *future = new Future<T>(worker_id, id, sharedData);
		return future;
};

template<typename T>
Future<T> *async(AsyncTask& f) {
	return async_impl<T>(1, f);
};

template<typename T>
Future<T> *async(unsigned int data_size, AsyncTask& f) {
	return async_impl<T>(data_size, f);
};

template <class T> Future<T>::Future(int _src_id, int _dst_id, 
																		communication::SharedDataManager *_sharedData) {
    ready_status = 0;
    src_id = _src_id;
		dst_id = _dst_id;
		sharedData = _sharedData;
};

template <class T> Future<T>::~Future() {
    Futures_Enviroment *env = Futures_Enviroment::Instance();
		int id = env->get_procId();
    if(id == dst_id) {
			delete sharedData;
		}
};

template <class T> bool Future<T>::is_ready() {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
		int id = env->get_procId();
		assert(id == dst_id);
    int ready_status;
    return sharedData->get_status(&ready_status);
};

template <class T> T Future<T>::get() {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
		int id = env->get_procId();
		assert(id == dst_id);
    int ready_status;
    while (1) {
        sharedData->get_status(&ready_status);
        if (ready_status) break;
    }
    return	details::_get_data<T>()(sharedData, details::_is_mpi_datatype<T>());
};

}//end of futures namespace

#endif

