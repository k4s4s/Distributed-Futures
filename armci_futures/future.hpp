
#ifndef Future_H
#define Future_H

#include <mpi.h>
extern "C" {
	#include <armci.h>
}

#include <iostream>
#include <unistd.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "futures_enviroment.hpp"

#ifdef ARMCI_MPI_V
	#define ARMCI_Access_begin(x) ARMCI_Access_begin(x) 
	#define ARMCI_Access_end(x) ARMCI_Access_end(x) 
#else
	#define ARMCI_Access_begin(x)
	#define ARMCI_Access_end(x)
#endif

template <class T>
class Future {
private:
		bool ready_status;
		unsigned int id; //id in the enviroment
		unsigned int data_size;
public:
    Future(unsigned int _data_size, unsigned int _type_size);
    ~Future();
		unsigned int get_Id();
    bool is_ready();
    T get(MPI_Datatype mpi_type);
};

template <class T> Future<T>::Future(unsigned int _data_size, unsigned int _type_size) {
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	data_size = _data_size;
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
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int *ready_status_buff = env->get_statusBuff(id);
	bool ready_status;
	ARMCI_Access_begin(ready_status_buff);
	ready_status = ready_status_buff[rank];
	ARMCI_Access_end(ready_status_buff);
	return ready_status;	
};

template <class T> T Future<T>::get(MPI_Datatype mpi_type) {
	/*try to get the value, if it is set, otherwise wait 'till it's set */
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	int rank;
	MPI_Comm_rank(env->get_communicator(), &rank);
	int **ready_status_buff = env->get_statusBuff(id);
	int *ready_status;
	while(1) { //spins until value is ready	
		ARMCI_Access_begin(ready_status_buff);
		ready_status = ready_status_buff[rank];
		ARMCI_Access_end(ready_status_buff);
		if(*ready_status) break;
	}	
	T *data = (T*)env->get_dataBuff(id);
	T value;
	ARMCI_Access_begin(data_buff);
	if(is_pointer<T>::value)
		value = data[rank];
	else
		value = *((T *)data[rank]);
	ARMCI_Access_end(data_buff);
	return value;
};

#endif

