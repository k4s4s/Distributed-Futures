
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <mpi.h>
extern "C" {
	#include <armci.h>
}

#include <stdlib.h>
#include <string> 
#include <map>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_MSG(str) std::cout << str << std::endl
#else
#define DEBUG_MSG(str) ;
#endif

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

template <class T>
class Futures_Enviroment { //singleton class
	private:
		MPI_Comm comm;
		MPI_Group group;
		T **data;
		unsigned int data_elements_num;
		int **status;
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
		T *get_dataPtr(int rank) { return data[rank]; };
		int *get_statusPtr(int rank) { return status[rank]; };
		MPI_Comm get_communicator() { return comm; };
		// this function returns the id used to register the future in the map
		unsigned int registerFuture();
		void removeFuture(unsigned int id);
		Future_Registry get_Future_Registry(unsigned int id);
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
	ARMCI_Init(); //perhaps check if it is already initialized
	MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &comm);
	data_elements_num = size;
	//Allocate shared memory
	int nprocs;
	MPI_Comm_size(comm, &nprocs);
	data = (T**)malloc(nprocs*sizeof(T*));
	ARMCI_Malloc((void **)data, size*sizeof(T));
	status = (int**)malloc(nprocs*sizeof(T*));
	ARMCI_Malloc((void **)status, size*sizeof(int));
};

template <class T> Futures_Enviroment<T>::~Futures_Enviroment() {
	int mpi_status;	
	MPI_Comm_free(&comm);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	ARMCI_Free(data[rank]);
	free(data);
	ARMCI_Free(status[rank]);
	free(status);
	ARMCI_Finalize(); //perhaps check if already finalized
	MPI_Finalized(&mpi_status);	
	if(!mpi_status) {
		MPI_Finalize();
	}
	pinstance = NULL;
};

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

