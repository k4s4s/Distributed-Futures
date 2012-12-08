
#ifndef Future_H
#define Future_H

#include <iostream>
#include <cassert>
#include "futures_enviroment.hpp"
#include "communication/communication.hpp"
#include "communication/mpi_details.hpp"
#include "details.hpp"
#include "future_fwd.hpp"
#include "common.hpp"
#include "apply_function.hpp"

namespace futures {

template <class T>
class Future {
private:
    int ready_status;
    T data;
    int src_id, dst_id;
    communication::SharedDataManager *sharedData;
public:
    Future(int _src_id, int _dst_id,
           communication::SharedDataManager *_sharedData);
    ~Future();
    bool is_ready();
    T get();
};

template<typename F, typename... Args>
class _async_stub : public _stub {
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & boost::serialization::base_object<_stub>(*this);
				ar & args;
    };
    F f;
  	std::tuple<Args...> args;
    typename std::result_of<F(Args...)>::type retVal;
public:
    _async_stub();
    _async_stub(F& _f, Args... _args);
    ~_async_stub();
    void run(int future_owner);
};

/** Implementation of async function **/
template<typename F, typename... Args>
Future<typename std::result_of<F(Args...)>::type> *async_impl(unsigned int data_size, F& f, Args... args) {
    DPRINT_MESSAGE("ASYNC:call to async");
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    int type_size = details::_sizeof<typename std::result_of<F(Args...)>::type>()
                    (details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>());
    int id = env->get_procId();
    //get worker id and wake him
    DPRINT_MESSAGE("ASYNC:issuing job");
    _stub *job = new _async_stub<F, Args...>(f, args...);
    int worker_id = env->get_avaibleWorker(job); //this call also wakes worker
    delete job;
    //send function object, worker runs it, get to init phase
    env->send_data(worker_id, data_size);
    env->send_data(worker_id, type_size);
    //create shared data on both ends, initial comm is over global communicator(?)
    DPRINT_MESSAGE("ASYNC:creating shared data");
    communication::SharedDataManager *sharedData;
    sharedData = env->new_SharedDataManager(worker_id, id, data_size, type_size,
                                            details::_get_mpi_datatype<typename std::result_of<F(Args...)>::type>()(
                                                    details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>()));
    //return future
    DPRINT_MESSAGE("ASYNC:returning future");
    Future<typename std::result_of<F(Args...)>::type> *future =
        new Future<typename std::result_of<F(Args...)>::type>(worker_id, id, sharedData);
    return future;
};

template<typename F, typename... Args>
Future<typename std::result_of<F(Args...)>::type> *async(F& f, Args... args) {
    return async_impl(1, f, args...);
};

template<typename F, typename... Args>
Future<typename std::result_of<F(Args...)>::type> *async(unsigned int data_size, F& f, Args... args) {
    return async_impl(data_size, f, args...);
};

/*** Future implementation ***/
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
    /* shared data is deleted at get*/
};

template <class T> bool Future<T>::is_ready() {
    if(ready_status) return true;
    Futures_Enviroment* env = Futures_Enviroment::Instance();
    int id = env->get_procId();
    assert(id == dst_id);
    return sharedData->get_status(&ready_status);
};

template <class T> T Future<T>::get() {
    if(ready_status) return data;
    Futures_Enviroment* env = Futures_Enviroment::Instance();
    int id = env->get_procId();
    assert(id == dst_id);
    while (1) {
        sharedData->get_status(&ready_status);
        if (ready_status) break;
    }
    data = details::_get_data<T>()(sharedData, details::_is_mpi_datatype<T>());
    delete sharedData;
    return data;
};

/*** _async_stub Implementation ***/
template <class F, class... Args>
_async_stub<F, Args...>::_async_stub() {};

template <class F, class... Args>
_async_stub<F, Args...>::_async_stub(F& _f, Args... _args): f(_f), args(_args...) {};

template <class F, class... Args>
_async_stub<F, Args...>::~_async_stub() {};

template <class F, class... Args>
void _async_stub<F, Args...>::run(int future_owner) {
    DPRINT_MESSAGE("JOB:initilizing job");
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    //get ids, data_size, type_size
    DPRINT_MESSAGE("JOB:receiving job data");
    int data_size = env->recv_data<int>(future_owner);
    int type_size = env->recv_data<int>(future_owner);
    int id = env->get_procId();
    //create shared data on both ends, initial comm is over global communicator(?)
    DPRINT_MESSAGE("JOB:creating shared data");
    communication::SharedDataManager *sharedData;
    sharedData = env->new_SharedDataManager(id, future_owner, data_size, type_size,
                                            details::_get_mpi_datatype<typename std::result_of<F(Args...)>::type>()(
                                                    details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>()));
    //execute work
    //retVal = f();
		retVal = apply(f, args);
    //return value to future
    details::_set_data<typename std::result_of<F(Args...)>::type>()(sharedData, retVal,
            details::_is_mpi_datatype<typename std::result_of<F(Args...)>::type>());
    int ready_status = 1;
    sharedData->set_status(&ready_status);
    delete sharedData;
};

}//end of futures namespace

#endif

