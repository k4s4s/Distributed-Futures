
#ifndef Future_H
#define Future_H

#include <iostream>
#include "futures_enviroment.hpp"
#include "communication/communication.hpp"

namespace futures {

template <class T>
class Future {
private:
    bool ready_status;
    unsigned int id; //id in the enviroment
public:
    Future(unsigned int _data_size, unsigned int _type_size);
    ~Future();
    unsigned int get_Id();
    bool is_ready();
    T get(MPI_Datatype mpi_type);
};

template<typename T, typename F, typename ... Args>
Future<T> *async(F& f, int origin_rank, int target_rank,
                 unsigned int data_size, unsigned int type_size, MPI_Datatype mpi_type,
                 Args ...args);

namespace details {

template<typename TX>
struct _get_data {
    TX operator()(communication::SharedDataManager* sharedDataManager, MPI_Datatype mpi_type) {
				TX value;				
				sharedDataManager->get_data(&value, mpi_type);
        return value;
    };
};

template<typename TX>
struct _get_data<TX*> {
    TX* operator()(communication::SharedDataManager* sharedDataManager, MPI_Datatype mpi_type) {
				TX* value = new TX[sharedDataManager->get_dataSize()];				
				sharedDataManager->get_data(&value, mpi_type);
        return value;
    };
};

template<typename TX>
struct _set_data {
    void operator()(communication::SharedDataManager* sharedDataManager, TX value,
										MPI_Datatype mpi_type, int rank) {
    	sharedDataManager->set_data(&value, mpi_type, rank);
    };
};

template<typename TX>
struct _set_data<TX*> {
    void operator()(communication::SharedDataManager* sharedDataManager, TX* value,
										MPI_Datatype mpi_type, int rank) {
    	sharedDataManager->get_data(value, mpi_type, rank);
    };
};

}//end of namespace details

template<typename T, typename F, typename ... Args>
Future<T> *async(F& f, int origin_rank, int target_rank,
                 unsigned int data_size, unsigned int type_size, MPI_Datatype mpi_type,
                 Args ...args) {
    Future<T> *future = new Future<T>(data_size, type_size);
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    //these should only be executed only by the thread that will set future's value
    int myrank = env->get_procId();
    if (myrank == origin_rank) {
        unsigned int future_id = future->get_Id();
        //F should be callable
        T retval = f(args...);
        communication::SharedDataManager* sharedDataManager = env->get_SharedDataManager(future_id);
				int ready_status = 1;
        sharedDataManager->set_status(&ready_status, target_rank);
        details::_set_data<T>()(sharedDataManager, retval, mpi_type, target_rank);
    }
    //Master and the rest should skip here directly
    return future; //the rest procs should move on... //FIXME: may return NULL if proc_rank != future_rank
};

template <class T> Future<T>::Future(unsigned int _data_size, unsigned int _type_size) {
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    ready_status = 0;
    id = env->registerFuture(_data_size, _type_size);
};

template <class T> Future<T>::~Future() {
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    env->removeFuture(id);
};

template <class T> unsigned int Future<T>::get_Id() {
    return id;
}

template <class T> bool Future<T>::is_ready() {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
    communication::SharedDataManager* sharedDataManager = env->get_SharedDataManager(id);
    int ready_status;
    return sharedDataManager->get_status(&ready_status);
};

template <class T> T Future<T>::get(MPI_Datatype mpi_type) {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
    communication::SharedDataManager* sharedDataManager = env->get_SharedDataManager(id);
    int ready_status;
    while (1) {
        sharedDataManager->get_status(&ready_status);
        if (ready_status) break;
    }
    return	details::_get_data<T>()(sharedDataManager, mpi_type);
};

}//end of futures namespace

#endif

