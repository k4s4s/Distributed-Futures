
#ifndef Promise_H
#define Promise_H

#include <mpi.h>
#include <iostream>
#include <typeinfo>
#include "future.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "futures_enviroment.hpp"
#include "communication/communication.hpp"

namespace futures {

template <class T> //implementation will probably work only for basic types (int, float, etc)
class Promise {
private:
    friend class boost::serialization::access;
    Future<T> *future; //maybe it sould not hold a copy of the object, there is no need for it, I think...
    int target_rank; //rank of the proc that holds the future
    unsigned int future_id; //registry of the future in the enviroment
    //MPI_Datatype mpi_type; //openmpi's MPI_Datatype is rather complicated, I can not serialize it
    //Maybe we could get similar wrappers as boost's for one-sided comm
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & target_rank & future_id ;
    };
public:
    Promise(int _origin_rank, int _target_rank, unsigned int _data_size, unsigned int _type_size);
    ~Promise();
    void set_value(T val, MPI_Datatype mpi_type);
    Future<T> *get_future();
};

template <class T> Promise<T>::Promise(int _origin_rank, int _target_rank, unsigned int _data_size, unsigned int _type_size) {
    future = new Future<T>(_data_size, _type_size);
    //these should only be executed only by the thread that will set future's value
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    int myrank = env->get_procId();
    if (myrank == _origin_rank) {
        future_id = future->get_Id();
        target_rank = _target_rank;
    }
};

template <class T> Promise<T>::~Promise() {};

template <class T> void Promise<T>::set_value(T val, MPI_Datatype mpi_type) {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
    communication::SharedDataManager* sharedDataManager = env->get_SharedDataManager(future_id);
    details::_set_data<T>()(sharedDataManager, val, mpi_type, target_rank);
		int ready_status = 1;
    sharedDataManager->set_status(&ready_status, target_rank);
};

template <class T> Future<T> *Promise<T>::get_future() {
    return future;
};

}//end of futures namespace

#endif

