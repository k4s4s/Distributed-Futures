
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
		mem::Shared_pointer shared_ptr;
		int type_size;
		int data_size;
public:
		future();
    future(int _src_id, int _dst_id, int _type_size, int _data_size,
					mem::Shared_pointer _shared_ptr);
		future(int _src_id, int _dst_id, T _data);
    ~future();
    bool is_ready();
    T get();
};

template<typename F, typename... Args>
class async_function : public _stub {
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(_stub);
				ar & BOOST_SERIALIZATION_NVP(dst_id) 
					& BOOST_SERIALIZATION_NVP(src_id) 
					& BOOST_SERIALIZATION_NVP(shared_ptr) 
					& BOOST_SERIALIZATION_NVP(data_size)
					& BOOST_SERIALIZATION_NVP(type_size)
					&	BOOST_SERIALIZATION_NVP(f) 
					& BOOST_SERIALIZATION_NVP(args);
    };
public:
		int src_id;
		int dst_id;
		mem::Shared_pointer shared_ptr;
		int data_size;
		int type_size;
    F f;
  	std::tuple<Args...> args;
    typename std::result_of<F(Args...)>::type retVal;
    async_function();
    async_function(int _src_id, int _dst_id, 
									mem::Shared_pointer _ptr, 
									int _data_size, int _type_size, 
									F& _f, Args... _args);
    ~async_function();
    void run();
};

/** Implementation of async function **/
template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async_impl(unsigned int data_size, F& f, Args... args) {
		INCREASE_COUNTER("total_jobs", 1); //FIXME: check if that's ok here
		START_TIMER("job_issue_time");
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    DPRINT_VAR("ASYNC:call to async from ", id);
    int type_size = details::_sizeof<typename std::result_of<F(Args...)>::type>()
                    (details::is_primitive_type<typename std::result_of<F(Args...)>::type>());

    //get worker id and wake him
		future<typename std::result_of<F(Args...)>::type> fut;
    int worker_id = env->get_avaibleWorker(); //this call also wakes worker

		if(worker_id == id && id ==  0) { //FIXME: checking if things can work without this code, just for master
		  DPRINT_VAR("\tASYNC:running on self", id);
			typename std::result_of<F(Args...)>::type retVal = f(args...); //run locally
			fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, retVal);
		}
		else {
			mem::Shared_pointer shared_ptr = env->alloc(id, type_size*data_size);		
		  _stub *job = new async_function<F, Args...>(worker_id, id, shared_ptr,
																									data_size, type_size, 
																									f, args...);		
			DPRINT_VAR("\tASYNC:scheduling on ", worker_id);
			fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, 
																															type_size, data_size,
																															shared_ptr);
			if(!env->schedule_job(worker_id, job)) { //it's possible to fail to schedule work on worker
		  	DPRINT_VAR("\tASYNC:failed to schedule job, running on self ", id);
				typename std::result_of<F(Args...)>::type retVal = f(args...); //run locally
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
												mem::Shared_pointer _shared_ptr) {
	src_id = _src_id;
	dst_id = _dst_id;
	type_size = _type_size;
	data_size = _data_size;
	shared_ptr = _shared_ptr;
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
    ready_status = env->get_data<int>(dst_id, shared_ptr, 1, STATUS_OFFSET);
		return ready_status;
};

template <class T> T future<T>::get() {
    if(ready_status) return data;
    Futures_Environment* env = Futures_Environment::Instance();
    int id = env->get_procId();
		//DPRINT_VAR("future.get():executing all pending jobs in queue:", id);
		//DPRINT_VAR("future.get():waiting...", ready_status);
		DPRINT_VAR("future.get():", id);
    //assert(id == dst_id);
		START_TIMER("idle_time");
    while (1) {
        ready_status = env->get_data<int>(dst_id, shared_ptr, 1, STATUS_OFFSET);
        if (ready_status) break;
				STOP_TIMER("idle_time");
				env->execute_pending_jobs(); //Maybe it's best to execute one job at a time
				START_TIMER("idle_time");
    }
		DPRINT_VAR("future.get():", ready_status);
		STOP_TIMER("idle_time");
		DPRINT_VAR("future.get():trying to retrieve data:", id);
    data = env->get_data<T>(dst_id, shared_ptr, data_size, DATA_OFFSET);
		env->free(id, shared_ptr);
		DPRINT_VAR("future.get():got data, returning it locally:", id);
    return data;
};

/*** async_function Implementation ***/
template <class F, class... Args>
async_function<F, Args...>::async_function() {};

template <class F, class... Args>
async_function<F, Args...>::async_function(int _src_id, int _dst_id, mem::Shared_pointer _ptr, 
																					int _data_size, int _type_size, F& _f, Args... _args):
	f(_f), args(_args...) {
	src_id = _src_id;
 	dst_id = _dst_id;
	shared_ptr = _ptr;
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
		START_TIMER("job_execution_time");
		functor_utils::apply(f, &retVal, args);
		STOP_TIMER("job_execution_time");
		DPRINT_VAR("JOB:Completed execution, setting future data ",id);
    //return value to future
		env->set_data(retVal, dst_id, shared_ptr, data_size, DATA_OFFSET);
		DPRINT_VAR("JOB:Set future data, setting status ",id);
    int ready_status = 1;
    env->set_data(ready_status, dst_id, shared_ptr, 1, STATUS_OFFSET);
		DPRINT_VAR("JOB:Set status, job completed ",id);
};

}//end of futures namespace

#endif

