
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <mpi.h>
#include <string> 
#include <map>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

class Future_Registry {
	private:
		unsigned int offset;
	public:
		friend class boost::serialization::access;
		Future_Registry() {};		
		Future_Registry(unsigned int _offset): offset(_offset) {};
		~Future_Registry() {};	
		template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */){
        ar & offset;
		};
		unsigned int get_offset() { return offset; }; 
};

//FIXME: need to find a way to have multiple futures(done), as well as of different types...
/* maybe we can have multiple enviroments in order to support different instantiation types(Not possible in
	current implementation though, since enviroment is a singleton) */

//TODO: Dynamically allocate window (is it possible to resize?) 

/*TODO: perhaps we could have the enviroment maintain another window, that will be used to contain futures, promises, 
				 thus using RMA puts/gets to emulate shared memory model more closely. An issue here can be serialization */
template <class T>
class Futures_Enviroment { //singleton class
	private:
		MPI_Comm comm;
		MPI_Win data_win;
		MPI_Win status_win;
		MPI_Group group;
		T* data;
		unsigned int data_elements_num;
		int* status;
		//std::map<std::string, MPI_Datatype> MPI_Datatypes;
		std::map<unsigned int, Future_Registry> futuresMap; /* hold information about different futures. Attention: it is possible that
		enviroment are different accross processes */
		unsigned int future_count;
		static Futures_Enviroment* pinstance; 
	protected:
		Futures_Enviroment(int &argc, char**& argv, unsigned int size);
	public:
		~Futures_Enviroment();
		static Futures_Enviroment* Instance(int &argc, char**& argv, unsigned int size);
		static Futures_Enviroment* Instance();
		MPI_Comm get_communicator();
	  MPI_Win get_data_window();
		MPI_Win get_status_window();
		MPI_Group get_group();
		T get_data();
		int get_status();
		// this function returns the id used to register the future in the map
		unsigned int registerFuture();
		void removeFuture(unsigned int id);
		Future_Registry get_Future_Registry(unsigned int id);
		//MPI_Datatype get_mpi_datatype(std::string &type_name);
};

template <class T> Futures_Enviroment<T>* Futures_Enviroment<T>::pinstance = NULL;// initialize pointer

template <class T> Futures_Enviroment<T>* Futures_Enviroment<T>::Instance (int &argc, char**& argv, unsigned int size) {
	if (!pinstance) {
		pinstance = new Futures_Enviroment(argc, argv, size); // create sole instance
	}
	return pinstance; // address of sole instance
};

template <class T> Futures_Enviroment<T>* Futures_Enviroment<T>::Instance () {

	return pinstance; // address of sole instance
};
	
template <class T> Futures_Enviroment<T>::Futures_Enviroment(int &argc, char**& argv, unsigned int size) {
	int mpi_status;
	MPI_Initialized(&mpi_status);
	if(!mpi_status) {
		MPI_Init(&argc, &argv);
	}
	MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &comm);
	data_elements_num = size;
	MPI_Alloc_mem(size, MPI_INFO_NULL, &data);
	MPI_Alloc_mem(size, MPI_INFO_NULL, &status);
	MPI_Win_create(data, size*sizeof(T), sizeof(T), MPI_INFO_NULL, comm, &data_win);
	MPI_Win_create(status, size*sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &status_win);
	//initialize mpi datatypes map, the fools way...
	/*
	MPI_Datatypes["char"] = MPI_CHAR;
	MPI_Datatypes["unsigned char"] = MPI_UNSIGNED_CHAR;
	MPI_Datatypes["byte"] = MPI_BYTE;
	MPI_Datatypes["short"] = MPI_SHORT;
	MPI_Datatypes["unsigned short"] = MPI_UNSIGNED_SHORT;
	MPI_Datatypes["int"] = MPI_INT;
	MPI_Datatypes["unsigned"] = MPI_UNSIGNED;
	MPI_Datatypes["long"] = MPI_LONG;
	MPI_Datatypes["unsigned long"] = MPI_UNSIGNED_LONG;
	MPI_Datatypes["float"] = MPI_FLOAT;
	MPI_Datatypes["double"] = MPI_DOUBLE;
	MPI_Datatypes["long double"] = MPI_LONG_DOUBLE;
	MPI_Datatypes["long long int"] = MPI_LONG_LONG_INT;
	MPI_Datatypes["float int"] = MPI_FLOAT_INT;
	MPI_Datatypes["double int"] = MPI_DOUBLE_INT;
	MPI_Datatypes["long int"] = MPI_LONG_INT;
	MPI_Datatypes["short int"] = MPI_SHORT_INT;
	MPI_Datatypes["long double int"] = MPI_LONG_DOUBLE_INT;
	//FIXME: only some types are supported, perhaps it's better to find out how boost deals with this
	*/
};

template <class T> Futures_Enviroment<T>::~Futures_Enviroment() {
	int mpi_status;	
	MPI_Win_free(&status_win);
	MPI_Win_free(&data_win);
	MPI_Comm_free(&comm);
	MPI_Finalized(&mpi_status);
	if(!mpi_status) {
		MPI_Finalize();
	}
	pinstance = NULL;
};

template <class T> MPI_Comm Futures_Enviroment<T>::get_communicator() {
	return comm;
}

template <class T> MPI_Win Futures_Enviroment<T>::get_data_window() {
	return data_win;
}

template <class T> MPI_Win Futures_Enviroment<T>::get_status_window() {
	return status_win;
}

template <class T> MPI_Group Futures_Enviroment<T>::get_group() {
	return group;
}

template <class T> T Futures_Enviroment<T>::get_data() {
	return data;
}

template <class T> int Futures_Enviroment<T>::get_status() {
	return status;
}

/*
template <class T> MPI_Datatype Futures_Enviroment<T>::get_mpi_datatype(std::string &type_name) {	
	return MPI_Datatypes[type_name];
}
*/

template <class T> unsigned int Futures_Enviroment<T>::registerFuture() {
	unsigned int id = future_count;
	future_count++;	
	Future_Registry reg(id);
	futuresMap[id] = reg;
	return id;
}

template <class T> void Futures_Enviroment<T>::removeFuture(unsigned int id) {
	futuresMap.erase(id);
}

template <class T> Future_Registry Futures_Enviroment<T>::get_Future_Registry(unsigned int id) {
	return futuresMap[id];
}

#endif

