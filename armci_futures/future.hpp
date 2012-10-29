
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

using namespace std;

//TODO: implement async 
template <class T>
class Future {
private:
		friend class boost::serialization::access;  
		int ready_status;
		T data; //note: value must be serializable
		unsigned int id; //id in the enviroment
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & ready_status & id;
		};
public:
    Future();
		Future(int _ready_status, T _data, unsigned int _id);
    ~Future();
    bool is_ready();
    T get();
		void set_id(unsigned int _id) { id = _id; };
};

template <class T> Future<T>::Future(): ready_status(false) {};

template <class T> Future<T>::Future(int _ready_status, T _data, unsigned int _id): 
ready_status(_ready_status), data(_data), id(_id) {};

template <class T> Future<T>::~Future() {};

template <class T> bool Future<T>::is_ready() {
	Futures_Enviroment<T> *env = Futures_Enviroment<T>::Instance();
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	unsigned int offset = env->get_Future_Registry(id).get_offset();
	int *ready_status = env->get_statusPtr(rank);
	bool status;
	ARMCI_Access_begin(ready_status);
	status = ready_status[offset];
	ARMCI_Access_end(ready_status);
	return status;	
};

template <class T> T Future<T>::get() {
	/*try to get the value, if it is set, otherwise wait 'till it's set */
	Futures_Enviroment<T> *env = Futures_Enviroment<T>::Instance();
	int rank;
	MPI_Comm_rank(env->get_communicator(), &rank);
	unsigned int offset = env->get_Future_Registry(id).get_offset();
	int *ready_status = env->get_statusPtr(rank);
	int status;
	while(1) { //spins until value is ready	
		ARMCI_Access_begin(ready_status);
		status = ready_status[offset];
		ARMCI_Access_end(ready_status);
		if(status) break;
	}	
	T *data = env->get_dataPtr(rank);
	T value;
	ARMCI_Access_begin(data);
	value = data[offset];
	cout << "data[offset]: " << data[offset] << endl;
	ARMCI_Access_end(data);
	return value;
};

#endif

