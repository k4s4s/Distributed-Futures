
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

template <class T>
class future {
private:
    int ready_status;
    T data;
    int src_id, dst_id;
    communication::Shared_data *sharedData;
public:
		future();
    future(int _src_id, int _dst_id,
           communication::Shared_data *_sharedData);
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
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & boost::serialization::base_object<_stub>(*this);
				ar & dst_id & src_id & base_address & data_size & type_size & args;
    };
		int src_id;
		int dst_id;
		int base_address;
		int data_size;
		int type_size;
    F f;
  	std::tuple<Args...> args;
    typename std::result_of<F(Args...)>::type retVal;
public:
    async_function();
    async_function(int _src_id, int _dst_id, int _base_address, 
									int _data_size, int _type_size, 
									F& _f, Args... _args);
    ~async_function();
    void run();
};

/** Implementation of async function **/
template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async_impl(unsigned int data_size, F& f, Args... args) {
		stats::StatManager *statManager = stats::StatManager::Instance();
		statManager->increase_total_jobs();
		statManager->start_timer("job_issue_time");
    DPRINT_MESSAGE("ASYNC:call to async");
    Futures_Environment *env = Futures_Environment::Instance();
    int type_size = details::_sizeof<typename std::result_of<F(Args...)>::type>()
                    (details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>());
    int id = env->get_procId();
    //get worker id and wake him
		future<typename std::result_of<F(Args...)>::type> fut;
    int worker_id = env->get_avaibleWorker(); //this call also wakes worker
		if(worker_id == id) {
		  DPRINT_MESSAGE("\tASYNC:running on self");
			typename std::result_of<F(Args...)>::type retVal = f(args...); //run locally
			fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, retVal);
		}
		else {
			communication::Shared_data *sharedData;
			int base_address = env->alloc(type_size*data_size);		
			sharedData = env->new_Shared_data(id, base_address, data_size, type_size, 
																				details::_get_mpi_datatype<typename std::result_of<F(Args...)>::type>()(
		                                    	details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>()),
																				env->get_data_window(), env->get_data_lock());
		  _stub *job = new async_function<F, Args...>(worker_id, id, base_address,
																									data_size, type_size, 
																									f, args...);		
			DPRINT_VAR("\tASYNC:scheduling on ", worker_id);
			fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, sharedData);
			if(!env->schedule_job(worker_id, job)) { //it's possible to fail to schedule work on worker
		  	DPRINT_MESSAGE("\tASYNC:failed to schedule job, running on self");
				typename std::result_of<F(Args...)>::type retVal = f(args...); //run locally
				fut = future<typename std::result_of<F(Args...)>::type>(worker_id, id, retVal);
			};
		}
		statManager->stop_timer("job_issue_time");
    return fut;
};

template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async(F& f, Args... args) {
    return async_impl(1, f, args...);
};

template<typename F, typename... Args>
future<typename std::result_of<F(Args...)>::type> async(unsigned int data_size, F& f, Args... args) {
    return async_impl(data_size, f, args...);
};

/*** future implementation ***/
template <class T> future<T>::future() {};

template <class T> future<T>::future(int _src_id, int _dst_id,
                                     communication::Shared_data *_sharedData) {
    ready_status = 0;
    src_id = _src_id;
    dst_id = _dst_id;
    sharedData = _sharedData;
};

template <class T> future<T>::future(int _src_id, int _dst_id,
                                     T _data) {
    ready_status = 1;
    src_id = _src_id;
    dst_id = _dst_id;
    data = _data;
};

template <class T> future<T>::~future() {
    /* shared data is deleted at get*/
};

template <class T> bool future<T>::is_ready() {
    if(ready_status) return true;
    Futures_Environment* env = Futures_Environment::Instance();
    int id = env->get_procId();
    assert(id == dst_id);
    return sharedData->get_status(&ready_status);
};

template <class T> T future<T>::get() {
    if(ready_status) return data;
    Futures_Environment* env = Futures_Environment::Instance();
    int id = env->get_procId();
    assert(id == dst_id);
		stats::StatManager *statManager = stats::StatManager::Instance();
		statManager->start_timer("idle_time");
    while (1) {
        sharedData->get_status(&ready_status);
        if (ready_status) break;
    }
		statManager->stop_timer("idle_time");
    data = details::_get_data<T>()(sharedData, details::_is_mpi_datatype<T>());
    return data;
};

/*** async_function Implementation ***/
template <class F, class... Args>
async_function<F, Args...>::async_function() {};

template <class F, class... Args>
async_function<F, Args...>::async_function(int _src_id, int _dst_id, int _base_address, 
																					int _data_size, int _type_size, F& _f, Args... _args):
	f(_f), args(_args...) {
	src_id = _src_id;
 	dst_id = _dst_id;
	base_address = _base_address;
	data_size = _data_size;
	type_size = _type_size;
};

template <class F, class... Args>
async_function<F, Args...>::~async_function() {};

template <class F, class... Args>
void async_function<F, Args...>::run() {
    DPRINT_MESSAGE("\tJOB:Running job on worker");
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    communication::Shared_data *sharedData;
    sharedData = env->new_Shared_data(dst_id, base_address, data_size, type_size,
                                      details::_get_mpi_datatype<typename std::result_of<F(Args...)>::type>()(
                                      	details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>()),
																			env->get_data_window(), env->get_data_lock());
    //execute work
    stats::StatManager *statManager = stats::StatManager::Instance();
		statManager->start_timer("job_execution_time");
		retVal = apply(f, args);
		statManager->stop_timer("job_execution_time");
    //return value to future
    details::_set_data<typename std::result_of<F(Args...)>::type>()(sharedData, retVal,
            details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>());
    int ready_status = 1;
    sharedData->set_status(&ready_status);
    delete sharedData;
};

}//end of futures namespace

#endif

