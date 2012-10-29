
#ifndef Promise_H
#define Promise_H

#include <mpi.h>
extern "C" {
	#include <armci.h>
}

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
		Future_Registry reg; //registry of the future in the enviroment
		//MPI_Datatype mpi_type; //openmpi's MPI_Datatype is rather complicated, I can not serialize it
		//Maybe we could get similar wrappers as boost's for one-sided comm
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & rank & reg;
		};
public:
		Promise() {};
    Promise(int _rank);
		Promise(int _rank, Future_Registry _reg);
    ~Promise();
    void set_value(T val);
    void set_value(T val, unsigned int n);
		Future<T> get_future();
};

template <class T> Promise<T>::Promise(int _rank):
    future(), rank(_rank)
{
/*		
		std::string type_name(typeid(T).name());
		mpi_type = Futures_Enviroment::Instance()->get_mpi_datatype(type_name);
*/
};

template <class T> Promise<T>::Promise(int _rank, Future_Registry _reg):
    future(NULL), rank(_rank), reg(_reg)
{
/*
		std::string type_name(typeid(T).name());
		mpi_type = Futures_Enviroment::Instance()->get_mpi_datatype(type_name);
*/
};

template <class T> Promise<T>::~Promise() {};

template <class T> void Promise<T>::set_value(T val) {
		/* We set remotely the future's value and then we set its flag to ready status*/
		Futures_Enviroment<T> *env = Futures_Enviroment<T>::Instance();
		T *data = env->get_dataPtr(rank);
		int offset = reg.get_offset();
		//set future data, probably need to lock this, however, it's the only one who modifie value, 
		//everyone else *ONLY ACCESSES* this data, so could be ok...
		ARMCI_Put(&val, &(data[offset]), sizeof(T), rank);
		//set future to ready status
		int ready_flag = 1;
		int *status = env->get_statusPtr(rank);
		ARMCI_Put(&ready_flag, &(status[offset]), sizeof(int), rank);		
};

template <class T> void Promise<T>::set_value(T val, unsigned int n) { // should be size*sizeof(Type)
		/* We set remotely the future's value and then we set its flag to ready status*/
		Futures_Enviroment<T> *env = Futures_Enviroment<T>::Instance();
		T *data = env->get_dataPtr(rank);
		int offset = reg.get_offset();
		//set future data, probably need to lock this, however, it's the only one who modifie value, 
		//everyone else *ONLY ACCESSES* this data, so could be ok...
		cout << "debug:val: " << val << "endl";
		ARMCI_Put(&val, &(data[offset]), n, rank);
		//set future to ready status
		int ready_flag = 1;
		int *status = env->get_statusPtr(rank);
		ARMCI_Put(&ready_flag, &(status[offset]), sizeof(int), rank);		
};

template <class T> Future<T> Promise<T>::get_future() {
		//TODO: remove member future object, just return it, all you need is id and rank
		Futures_Enviroment<T>* env = Futures_Enviroment<T>::Instance();
		unsigned int id = env->registerFuture();
		future.set_id(id);
		reg = env->get_Future_Registry(id);
		return future;		
};

#endif

