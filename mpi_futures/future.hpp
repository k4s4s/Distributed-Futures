
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

using namespace std;

//TODO: need to address polymorphism, template is not enough, we need to specify mpi data type 
//TODO: implement async 
//TODO: ?
template <class T>
class Future {
private:
		bool ready_status;
		unsigned int id; //id in the enviroment
		unsigned int data_size;
public:
    Future();
		Future(bool _ready_status, unsigned int _id, unsigned int _data_size);
    ~Future();
    bool is_ready();
    T get(MPI_Datatype mpi_type);
		void set_id(unsigned int _id) { id = _id; };
		void set_data_size(unsigned int _data_size) { data_size = _data_size; };
};

template <class T> Future<T>::Future(): ready_status(false) {};

template <class T> Future<T>::Future(bool _ready_status, unsigned int _id, unsigned int _data_size): 
ready_status(_ready_status), id(_id), data_size(_data_size) {};

template <class T> Future<T>::~Future() {};

template <class T> bool Future<T>::is_ready() {
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	MPI_Win status_win = env->get_status_window();
	int rank;
	MPI_Comm_rank(env->get_communicator(), &rank);
	unsigned int offset = env->get_Future_Registry(id).get_offset();
	//if we don't lock here, mpi stays in an infinite loop and cannot unlock on promise's side
	MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);
	MPI_Get(&ready_status, 1, MPI_INT, rank, offset, 1, MPI_INT, status_win);
	MPI_Win_unlock(rank, status_win);
	return ready_status;	
};

template <class T> T Future<T>::get(MPI_Datatype mpi_type) {
	/*try to get the value, if it is set, otherwise wait 'till it's set */
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	MPI_Win status_win = env->get_status_window();
	int rank;
	MPI_Comm_rank(env->get_communicator(), &rank);
	unsigned int offset = env->get_Future_Registry(id).get_offset();
	while(1) { //spins until value is ready	
		//if we don't lock here, mpi stays in an infinite loop and cannot unlock on promise's side
		MPI_Win_lock(MPI_LOCK_SHARED, rank, 0, status_win);
		MPI_Get(&ready_status, 1, MPI_INT, rank, offset, 1, MPI_INT, status_win);
		MPI_Win_unlock(rank, status_win);
		if(ready_status) break;
	}	
	//MPI specification says to lock even local accesses, I think in our case we can remove this...
	T value;
	if(is_pointer<T>::value) {
		value = (T)malloc(sizeof(*value)*data_size); //FIXME:
		MPI_Win data_win = env->get_data_window();
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, data_win);		
		MPI_Get(value, data_size, mpi_type, rank, offset, data_size, mpi_type, data_win);
		MPI_Win_unlock(rank, data_win);
		return value;
	}
	else {
		MPI_Win data_win = env->get_data_window();
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, rank, 0, data_win);
		//no need for data_size here, we only get 1 element since scalar
		MPI_Get(&value, data_size, mpi_type, rank, offset, data_size, mpi_type, data_win);
		MPI_Win_unlock(rank, data_win);
		return value;
	}
	return value;
};

#endif

