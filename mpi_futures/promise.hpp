
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

using namespace std;

template <class T> //implementation will probably work only for basic types (int, float, etc)
class Promise {
private:
		friend class boost::serialization::access;
		Future<T> future; //maybe it sould not hold a copy of the object, there is no need for it, I think...
		int rank; //rank of the proc that holds the future
		unsigned int offset; //registry of the future in the enviroment
		unsigned int data_size;
		//MPI_Datatype mpi_type; //openmpi's MPI_Datatype is rather complicated, I can not serialize it
		//Maybe we could get similar wrappers as boost's for one-sided comm
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & rank & offset & data_size;
		};
public:
		Promise() {};
    Promise(int _rank);
		Promise(int _rank, unsigned int _data_size);
		Promise(int _rank, unsigned int _offset, unsigned int _data_size);
    ~Promise();
    void set_value(T val, MPI_Datatype mpi_type);
		Future<T> get_future();
};

template <class T> Promise<T>::Promise(int _rank):
    future(), rank(_rank), data_size(1) {};


template <class T> Promise<T>::Promise(int _rank, unsigned int _data_size):
    future(), rank(_rank), data_size(_data_size) {};

template <class T> Promise<T>::Promise(int _rank, unsigned int _offset, unsigned int _data_size):
    rank(_rank), offset(_offset), data_size(_data_size) {};

template <class T> Promise<T>::~Promise() {};

template <class T> void Promise<T>::set_value(T val, MPI_Datatype mpi_type) {
		/* We set remotely the future's value and then we set its flag to ready status*/
		Futures_Enviroment *env = Futures_Enviroment::Instance();
		MPI_Win data_win = env->get_data_window();
		//set future data
		if(is_pointer<T>::value) {
			MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, data_win);
			MPI_Put(val, data_size, mpi_type, rank, offset, data_size, mpi_type, data_win);
			MPI_Win_unlock(rank, data_win);
		}
		else {
			MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, data_win);
			MPI_Put(&val, data_size, mpi_type, rank, offset, data_size, mpi_type, data_win);
			MPI_Win_unlock(rank, data_win);
		}
		//set future to ready status
		MPI_Win status_win = env->get_status_window();
		int ready_flag = 1;
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, status_win);		
		MPI_Put(&ready_flag, 1, MPI_INT, rank, offset, 1, MPI_INT, status_win);
		MPI_Win_unlock(rank, status_win);
};

template <class T> Future<T> Promise<T>::get_future() {
		Futures_Enviroment* env = Futures_Enviroment::Instance();
		unsigned int id = env->registerFuture(); //FIXME: revise this section, data_size exists in multiple sides
		future.set_id(id);
		future.set_data_size(data_size);
		offset = id;
		return future;		
};

#endif

