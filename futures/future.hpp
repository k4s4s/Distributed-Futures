
#ifndef Future_H
#define Future_H

#include <iostream>
#include <cassert>
#include "futures_enviroment.hpp"
#include "communication/communication.hpp"

namespace futures {

template <class T, class DataT>
class AsyncTask {
public:
	virtual ~AsyncTask() {};
	virtual T operator()(DataT args) = 0; 
};

template <class T>
class Future {
private:
    bool ready_status;
<<<<<<< HEAD
    int src_id, dst_id;
		communication::SharedDataManager *sharedData;
public:
		Future(int _src_id, int _dst_id, 
					communication::SharedDataManager *_sharedData);
=======
    unsigned int id; //id in the enviroment
		int promise_rank;
public:
    Future(unsigned int _data_size, unsigned int _type_size, int _promise_rank);
>>>>>>> mpi-async-comm
    ~Future();
    bool is_ready();
    T get();
};

template<typename T, typename F, typename ... Args>
Future<T> *async(int src_id, int dst_id,
                 unsigned int data_size, unsigned int type_size,
                 F& f, Args ...args);

namespace details {

template<typename TX>
struct _get_data {
    TX operator()(communication::SharedDataManager* sharedDataManager, int rank) {
				TX value;				
				sharedDataManager->get_data(&value, rank);
        return value;
    };
};

template<typename TX>
struct _get_data<TX*> {
    TX* operator()(communication::SharedDataManager* sharedDataManager, int rank) {
				TX* value = new TX[sharedDataManager->get_dataSize()];				
				sharedDataManager->get_data(value, rank);
        return value;
    };
};

template<typename TX>
struct _set_data {
    void operator()(communication::SharedDataManager* sharedDataManager,
										TX value) {
    	sharedDataManager->set_data(&value);
    };
};

template<typename TX>
struct _set_data<TX*> {
    void operator()(communication::SharedDataManager* sharedDataManager, 
										TX* value) {
    	sharedDataManager->set_data(value);
    };
};

}//end of namespace details

template<typename T, typename F, typename ... Args>
Future<T> *async(int src_id, int dst_id,
                 unsigned int data_size, unsigned int type_size,
                 F& f, Args ...args) {
<<<<<<< HEAD
=======
    Future<T> *future = new Future<T>(data_size, type_size, origin_rank);
>>>>>>> mpi-async-comm
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    int id = env->get_procId();
		if(id == src_id || id == dst_id) {
			communication::SharedDataManager *sharedData;
			sharedData = env->new_SharedDataManager(src_id, dst_id, data_size, type_size);
			if(id == dst_id) {
				Future<T> *future = new Future<T>(src_id, dst_id, sharedData);
				return future;
			}
			else if(id == src_id) {
        T retval = f(args...);
        details::_set_data<T>()(sharedData, retval);
				int ready_status = 1;
        sharedData->set_status(&ready_status);
				//delete sharedData;
			}
		}
    //Master and the rest should skip here directly
    return NULL;//the rest procs should move on... //FIXME: may return NULL if proc_rank != future_rank
};

<<<<<<< HEAD
template <class T> Future<T>::Future(int _src_id, int _dst_id, 
																		communication::SharedDataManager *_sharedData) {
    ready_status = 0;
    src_id = _src_id;
		dst_id = _dst_id;
		sharedData = _sharedData;
=======
/*** Future Implementation ***/
template <class T> Future<T>::Future(unsigned int _data_size, unsigned int _type_size, int _promise_rank) {
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    ready_status = 0;
    id = env->registerFuture(_data_size, _type_size);
		promise_rank = _promise_rank;
>>>>>>> mpi-async-comm
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
<<<<<<< HEAD
		int id = env->get_procId();
		assert(id == dst_id);
    int ready_status;
    return sharedData->get_status(&ready_status);
=======
    communication::SharedDataManager* sharedDataManager = env->get_SharedDataManager(id);
    int ready_status = 0;
    return sharedDataManager->get_status(&ready_status, promise_rank);
>>>>>>> mpi-async-comm
};

template <class T> T Future<T>::get() {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
<<<<<<< HEAD
		int id = env->get_procId();
		assert(id == dst_id);
    int ready_status;
    while (1) {
        sharedData->get_status(&ready_status);
        if (ready_status) break;
    }
    return	details::_get_data<T>()(sharedData);
=======
    communication::SharedDataManager* sharedDataManager = env->get_SharedDataManager(id);
    int ready_status = 0;
    while (1) {
        sharedDataManager->get_status(&ready_status, promise_rank);
        if (ready_status) break;
    }
    return	details::_get_data<T>()(sharedDataManager, promise_rank);
>>>>>>> mpi-async-comm
};

}//end of futures namespace

#endif

