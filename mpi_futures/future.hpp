
#ifndef Future_H
#define Future_H

#include <mpi.h>
#include <iostream>

#include <unistd.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "futures_enviroment.hpp"

template <class T>
class Future {
private:
		bool ready_status;
		unsigned int id; //id in the enviroment
		unsigned int data_size;
		unsigned int type_size;
public:
    Future(unsigned int _data_size, unsigned int _type_size);
    ~Future();
		unsigned int get_Id();
    bool is_ready();
    T get(MPI_Datatype mpi_type);
protected:
	template<typename TX> 
		struct _get { 
			TX operator()(MPI_Datatype mpi_type, bool ready_status, unsigned int id, unsigned int data_size, unsigned int type_size) { 
				/*try to get the value, if it is set, otherwise wait 'till it's set */
				Futures_Enviroment *env = Futures_Enviroment::Instance();
				MPI_Win status_win = env->get_statusWindow(id);
				int rank;
				MPI_Comm_rank(env->get_communicator(), &rank);
				while(1) { //spins until value is ready	
					//if we don't lock here, mpi stays in an infinite loop and cannot unlock on promise's side
					MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);
					MPI_Get(&ready_status, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win);
					MPI_Win_unlock(rank, status_win);
					if(ready_status) break;
				}	
				//MPI specification says to lock even local accesses, I think in our case we can remove this...
				TX value;
				MPI_Win data_win = env->get_dataWindow(id);
				//value = malloc(data_size*type_size);
				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, data_win);		
				MPI_Get(&value, data_size, mpi_type, rank, 0, data_size, mpi_type, data_win);
				MPI_Win_unlock(rank, data_win);
				return value;
			}
		};

	template<typename TX> 
		struct _get<TX*> { 
			TX* operator()(MPI_Datatype mpi_type, bool ready_status, unsigned int id, unsigned int data_size, unsigned int type_size) { 
				/*try to get the value, if it is set, otherwise wait 'till it's set */
				Futures_Enviroment *env = Futures_Enviroment::Instance();
				MPI_Win status_win = env->get_statusWindow(id);
				int rank;
				MPI_Comm_rank(env->get_communicator(), &rank);
				while(1) { //spins until value is ready	
					//if we don't lock here, mpi stays in an infinite loop and cannot unlock on promise's side
					MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);
					MPI_Get(&ready_status, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win);
					MPI_Win_unlock(rank, status_win);
					if(ready_status) break;
				}	
				//MPI specification says to lock even local accesses, I think in our case we can remove this...
				TX* value;
				MPI_Win data_win = env->get_dataWindow(id);
				value = (TX*)malloc(data_size*type_size);
				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, data_win);		
				MPI_Get(value, data_size, mpi_type, rank, 0, data_size, mpi_type, data_win);
				MPI_Win_unlock(rank, data_win);
				return value;
			}
		};
};

template <class T> Future<T>::Future(unsigned int _data_size, unsigned int _type_size) {
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	data_size = _data_size;
	type_size = _type_size;
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
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	MPI_Win status_win = env->get_statusWindow(id);
	int rank;
	MPI_Comm_rank(env->get_communicator(), &rank);
	//if we don't lock here, mpi stays in an infinite loop and cannot unlock on promise's side
	MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);
	MPI_Get(&ready_status, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win);
	MPI_Win_unlock(rank, status_win);
	return ready_status;	
};

template <class T> T Future<T>::get(MPI_Datatype mpi_type) {
	return _get<T>()(mpi_type, ready_status, id, data_size, type_size);
};

#endif

