
#ifndef future_H
#define future_H

#include <iostream>
#include <cassert>
#include "futures_environment.hpp"
#include "communication/communication.hpp"
#include "communication/mpi_details.hpp"
#include "details.hpp"
#include "future_fwd.hpp"
#include "common.hpp"
#include "apply_function.hpp"

namespace futures {

/** \brief future object class
 * The futures object class which is used to retrieve
 * a value after issuing an async call
*/
template <class T>
class future {
private:
    int ready_status;
    T data;
    int src_id, dst_id;
		mem::Shared_pointer status_ptr;
		mem::Shared_pointer data_ptr;
		int type_size;
		int data_size;
public:
		future();
    future(int _src_id, int _dst_id, int _type_size, int _data_size,
					mem::Shared_pointer& _status_ptr, mem::Shared_pointer& _data_ptr);
		future(int _src_id, int _dst_id, T _data);
    ~future();
    bool is_ready();
    T get();
    template<class Archive>
    void serialize(Archive & ar) {
			ar(	ready_status, data, src_id, dst_id,
					status_ptr, data_ptr, type_size, data_size);
		}
};

template<typename F, typename... Args>
class async_function : public _stub {
public:
		int src_id;
		int dst_id;
		mem::Shared_pointer status_ptr;
		mem::Shared_pointer data_ptr;
		int data_size;
		int type_size;
    F f;
  	std::tuple<Args...> args;
    typename std::result_of<F(Args...)>::type retVal;
    async_function();
    async_function(int _src_id, int _dst_id, 
									mem::Shared_pointer _status_ptr,
									mem::Shared_pointer _data_ptr, 
									int _data_size, int _type_size, 
									F& _f, Args... _args);
    ~async_function();
    void run();
	 	template<class Archive>
		void serialize(Archive & ar) {
			ar(	cereal::virtual_base_class<_stub>(this),
					dst_id, src_id, status_ptr, 
					data_ptr, data_size, type_size, f, args);
		};
};

/** Implementation of make_future function **/
template<typename T>
future<T> make_future(T val) {
	STOP_TIMER("user_code_execution_time");
  Futures_Environment *env = Futures_Environment::Instance();
  int id = env->get_procId();
	future<T> fut(id, id, val);
	START_TIMER("user_code_execution_time");
	return fut;
}

/** Implementation of async function **/
template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async_impl(unsigned int data_size, F& f, Args... args) {
		STOP_TIMER("user_code_execution_time");
		INCREASE_COUNTER("total_jobs", 1); //FIXME: check if that's ok here
		START_TIMER("job_issue_time");
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    DPRINT_VAR("ASYNC:call to async from ", id);
    //get worker id and wake him
		future<typename std::result_of<F(Args...)>::type> fut;
    int worker_id = env->get_avaibleWorker(); //this call also wakes worker

		if(worker_id == id && id ==  0) { //FIXME: checking if things can work without this code, just for master
			STOP_TIMER("job_issue_time");
		  DPRINT_VAR("\tASYNC:running on self", id);
			START_TIMER("user_code_execution_time");
			typename std::result_of<F(Args...)>::type retVal = f(args...); //run locally
			STOP_TIMER("user_code_execution_time");
			START_TIMER("job_issue_time");
			fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, retVal);
		}
		else {
			/* 
				if return type is not a container, then we cannot know apriori the size 
				that the function will return, so the caller will need to retrieve data
				from the worker instead of the worker setting data on the caller
			*/
			int type_size;
			mem::Shared_pointer status_ptr, data_ptr;
			if(!details::is_container<typename std::result_of<F(Args...)>::type>::value) {
				type_size = details::_sizeof<typename std::result_of<F(Args...)>::type>()
						      (details::is_primitive_type<typename std::result_of<F(Args...)>::type>());
				status_ptr = env->alloc(id, sizeof(int));
				data_ptr = env->alloc(id, type_size*data_size);
			}
			else {
				type_size = details::_sizeof<typename std::result_of<F(Args...)>::type>()
						      (details::is_primitive_type<typename std::result_of<F(Args...)>::type>());
				status_ptr = env->alloc(id, sizeof(int));
				data_ptr = env->alloc(id, type_size*data_size);
			}	
		  std::shared_ptr<_stub> job = std::make_shared<async_function<F, Args...>>(worker_id, id, 
																																								status_ptr, data_ptr,
																																								data_size, type_size, 
																																								f, args...);		
			DPRINT_VAR("\tASYNC:scheduling on ", worker_id);
			fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, 
																															type_size, data_size,
																															status_ptr, data_ptr);
			if(!env->schedule_job(worker_id, job)) { //it's possible to fail to schedule work on worker
				STOP_TIMER("job_issue_time");
		  	DPRINT_VAR("\tASYNC:failed to schedule job, running on self ", id);
				START_TIMER("user_code_execution_time");
				typename std::result_of<F(Args...)>::type retVal = f(args...); //run locally
				STOP_TIMER("user_code_execution_time");
				START_TIMER("job_issue_time");
				fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, retVal);
			};
		}
		STOP_TIMER("job_issue_time");
		DPRINT_VAR("ASYNC:returning future ", id);
    return fut;
};

template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async(F& f, Args... args) {
    return async_impl(1, f, args...);
};

template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async2(unsigned int data_size, F& f, Args... args) {
    return async_impl(data_size, f, args...);
};

/*** future implementation ***/
template <class T> future<T>::future() {};

template <class T> future<T>::future(int _src_id, int _dst_id, int _type_size, int _data_size,
												mem::Shared_pointer& _status_ptr, mem::Shared_pointer& _data_ptr) {
	src_id = _src_id;
	dst_id = _dst_id;
	type_size = _type_size;
	data_size = _data_size;
	status_ptr = _status_ptr;
	data_ptr = _data_ptr;
	ready_status = 0;
};

template <class T> future<T>::future(int _src_id, int _dst_id,
                                     T _data) {
    ready_status = 1;
    src_id = _src_id;
    dst_id = _dst_id;
    data = _data;
};

template <class T> future<T>::~future() {};

template <class T> bool future<T>::is_ready() {
    if(ready_status) return true;
    Futures_Environment* env = Futures_Environment::Instance();
    ready_status = env->get_data<int>(status_ptr.node_id, status_ptr, 1, 0);
		return ready_status;
};

template <class T> T future<T>::get() {
    Futures_Environment* env = Futures_Environment::Instance();
    int id = env->get_procId();

		STOP_TIMER("user_code_execution_time");

    if(ready_status) {
			START_TIMER("user_code_execution_time");
			return data;
		}
		//DPRINT_VAR("future.get():executing all pending jobs in queue:", id);
		//DPRINT_VAR("future.get():waiting...", ready_status);
		DPRINT_VAR("future.get():", id);
    //assert(id == dst_id);
		START_TIMER("idle_time");
    while (1) {
        ready_status = env->get_data<int>(status_ptr.node_id, status_ptr, 1, 0);
        if (ready_status) break;
				env->execute_pending_jobs(); //Maybe it's best to execute one job at a time
    }
		DPRINT_VAR("future.get():", ready_status);
		STOP_TIMER("idle_time");
		DPRINT_VAR("future.get():trying to retrieve data:", id);
		//TODO: here we need to handle containers differently
    data = env->get_data<T>(data_ptr.node_id, data_ptr, data_size, 0);
		env->free(data_ptr.node_id, data_ptr);
		env->free(status_ptr.node_id, status_ptr);
		DPRINT_VAR("future.get():got data, returning it locally:", id);
		START_TIMER("user_code_execution_time");
    return data;
};

/*** async_function Implementation ***/
template <class F, class... Args>
async_function<F, Args...>::async_function() {};

template <class F, class... Args>
async_function<F, Args...>::async_function(int _src_id, int _dst_id, 
																					mem::Shared_pointer _status_ptr,
																					mem::Shared_pointer _data_ptr, 
																					int _data_size, int _type_size, F& _f, Args... _args):
	f(_f), args(_args...) {
	src_id = _src_id;
 	dst_id = _dst_id;
	status_ptr = _status_ptr;
	data_ptr = _data_ptr;
	data_size = _data_size;
	type_size = _type_size;
};

template <class F, class... Args>
async_function<F, Args...>::~async_function() {};

template <class F, class... Args>
void async_function<F, Args...>::run() {
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    DPRINT_VAR("JOB:Running job on worker", id);
    //execute work
		START_TIMER("user_code_execution_time");
		functor_utils::apply(f, &retVal, args);
		STOP_TIMER("user_code_execution_time");
		DPRINT_VAR("JOB:Completed execution, setting future data ",id);
		START_TIMER("value_return_time");
    //return value to future
		//TODO: here we need to handle containers differently 
		env->set_data(retVal, data_ptr.node_id, data_ptr, data_size, 0);
		DPRINT_VAR("JOB:Set future data, setting status ",id);
    int ready_status = 1;
    env->set_data(ready_status, status_ptr.node_id, status_ptr, 1, 0);
		DPRINT_VAR("JOB:Set status, job completed ",id);
		STOP_TIMER("value_return_time");
};

}//end of futures namespace

#endif

