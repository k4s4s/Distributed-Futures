
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

template <class T> //implementation will probably work only for basic types (int, float, etc)
class Promise {
private:
		friend class boost::serialization::access;
		Future<T> *future; //maybe it sould not hold a copy of the object, there is no need for it, I think...
		int rank; //rank of the proc that holds the future
		unsigned int future_id; //registry of the future in the enviroment
		unsigned int data_size;
		//MPI_Datatype mpi_type; //openmpi's MPI_Datatype is rather complicated, I can not serialize it
		//Maybe we could get similar wrappers as boost's for one-sided comm
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & rank & future_id << data_size;
		};
public:
		Promise(int _rank, int _myrank, unsigned int _data_size, unsigned int _type_size);
    ~Promise();
    void set_value(T val, MPI_Datatype mpi_type);
		Future<T> *get_future();
protected:
	template<typename TX> 
		struct _set_value { 
			void operator()(TX val, MPI_Datatype mpi_type, int rank, unsigned int future_id, unsigned int data_size) {
				/* We set remotely the future's value and then we set its flag to ready status*/
				Futures_Enviroment *env = Futures_Enviroment::Instance();
				MPI_Win data_win = env->get_dataWindow(future_id);
				//set future data
				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, data_win);
				MPI_Put(&val, data_size, mpi_type, rank, 0, data_size, mpi_type, data_win);
				MPI_Win_unlock(rank, data_win);
				//set future to ready status
				MPI_Win status_win = env->get_statusWindow(future_id);
				int ready_flag = 1;
				MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);		
				MPI_Put(&ready_flag, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win);
				MPI_Win_unlock(rank, status_win);	
			}
		};

	template<typename TX> 
		struct _set_value<TX*> { 
			void operator()(TX* val, MPI_Datatype mpi_type, int rank, unsigned int future_id, unsigned int data_size) {
				/* We set remotely the future's value and then we set its flag to ready status*/
				Futures_Enviroment *env = Futures_Enviroment::Instance();
				MPI_Win data_win = env->get_dataWindow(future_id);
				//set future data
				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, data_win);
				MPI_Put(val, data_size, mpi_type, rank, 0, data_size, mpi_type, data_win);
				MPI_Win_unlock(rank, data_win);
				//set future to ready status
				MPI_Win status_win = env->get_statusWindow(future_id);
				int ready_flag = 1;
				MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);		
				MPI_Put(&ready_flag, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win);
				MPI_Win_unlock(rank, status_win);				
			}
		};
};

template <class T> Promise<T>::Promise(int _rank, int _myrank, unsigned int _data_size, unsigned int _type_size) {
	future = new Future<T>(_data_size, _type_size);
	//these should only be executed only by the thread that will set future's value
	int myrank;
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	MPI_Comm_rank(env->get_communicator(), &myrank);
	if(myrank == _myrank) {	
		future_id = future->get_Id();
		data_size = _data_size;
		rank = _rank;
	}
};

template <class T> Promise<T>::~Promise() {};

template <class T> void Promise<T>::set_value(T val, MPI_Datatype mpi_type) {
	_set_value<T>()(val, mpi_type, rank, future_id, data_size);
};

template <class T> Future<T> *Promise<T>::get_future() {
		return future;		
};

#endif

