
#ifndef Promise_H
#define Promise_H

#include <iostream>
#include <cassert>
#include "futures_environment.hpp"
#include "communication/communication.hpp"
#include "communication/mpi_details.hpp"
#include "future.hpp"

namespace futures {

template <class T>
class Promise {
private:
    future<T> fut; //this is only valid on destination, NULL otherwise
    /*these values also exist on future and sharedDataManager, maybe keep them in one place?,
    	could use inline functions to get them maybe? */
    int src_id, dst_id;
    communication::SharedDataManager *sharedData;
public:
    Promise(int _src_rank, int _dst_rank, unsigned int _data_size);
    Promise(int _src_rank, int _dst_rank);
    ~Promise();
    void set_value(T val);
    future<T> get_future();
};

template <class T> Promise<T>::Promise(int _src_id, int _dst_id,
                                       unsigned int _data_size) {
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    src_id = _src_id;
    dst_id = _dst_id;
    unsigned int _type_size = details::_sizeof<T>()(details::_is_mpi_datatype<T>());
    if(id == _src_id || id == _dst_id) {
        sharedData = env->new_SharedDataManager(_src_id, _dst_id, _data_size, _type_size,
                                                details::_get_mpi_datatype<T>()(
                                                        details::_is_mpi_datatype<T>()));
        if(id == _dst_id)
            fut = future<T>(_src_id, _dst_id, sharedData);
        else
            fut = NULL;
    }
    else
        fut = NULL;
};

template <class T> Promise<T>::Promise(int _src_id, int _dst_id) {
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    src_id = _src_id;
    dst_id = _dst_id;
    unsigned int _type_size = details::_sizeof<T>()(details::_is_mpi_datatype<T>());
    if(id == _src_id || id == _dst_id) {
        sharedData = env->new_SharedDataManager(_src_id, _dst_id, 1, _type_size,
                                                details::_get_mpi_datatype<T>()(
                                                        details::_is_mpi_datatype<T>()));
        if(id == _dst_id)
            fut = future<T>(_src_id, _dst_id, sharedData);
        else
            fut = NULL;
    }
    else
        fut = NULL;
};

template <class T> Promise<T>::~Promise() {
    Futures_Environment *env = Futures_Environment::Instance();
    int id = env->get_procId();
    if(id == src_id) {
        delete sharedData;
    } //future should be deleted by user program
};

template <class T> void Promise<T>::set_value(T val) {
    Futures_Environment* env = Futures_Environment::Instance();
    int id = env->get_procId();
    assert(id == src_id);
    details::_set_data<T>()(sharedData, val, details::_is_mpi_datatype<T>());
    int ready_status = 1;
    sharedData->set_status(&ready_status);
};

template <class T> future<T> Promise<T>::get_future() {
    //Futures_Environment* env = Futures_Environment::Instance();
    //int id = env->get_procId();
    //assert(id == dst_id);
    return fut;
};

}//end of futures namespace

#endif

