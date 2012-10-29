
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

#ifdef DEBUG
#define DEBUG_MSG(str) std::cout << str << std::endl
#else
#define DEBUG_MSG(str) ;
#endif

class Future_Registry {
	private:
		void *data_ptr;
		unsigned int data_size;
		//MPI_Datatype datatype;
		unsigned int offset;
	public:
		Future_Registry() {};		
		Future_Registry(void *_data_ptr, unsigned int _data_size, unsigned int _offset): 
		data_ptr(_data_ptr), data_size(_data_size), offset(_offset) {};
		~Future_Registry() {};	
		void *get_dataPtr() { return data_ptr; };
		unsigned int get_dataSize() { return data_size; };
		unsigned int get_offset() { return offset; }; 
};



//FIXME: need to find a way to have multiple futures(done), as well as of different types...
/* maybe we can have multiple enviroments in order to support different instantiation types(Not possible in
	current implementation though, since enviroment is a singleton) */

class Futures_Enviroment { //singleton class
	private:
		MPI_Comm comm;
		MPI_Group group;
		void ***data;
		int **status;
		std::map<unsigned int, Future_Registry> futuresMap; /* hold information about different futures. Attention: it is possible that
		enviroment are different accross processes */
		unsigned int max_futures;
		unsigned int future_count;
		static Futures_Enviroment* pinstance; 
	protected:
		Futures_Enviroment(int &argc, char**& argv, unsigned int futures_number);
	public:
		~Futures_Enviroment();
		static Futures_Enviroment* Instance(int &argc, char**& argv, unsigned int futures_number);
		static Futures_Enviroment* Instance();
		void *get_dataPtr(int rank) { return data[rank]; };
		int *get_statusPtr(int rank) { return status[rank]; };
		MPI_Comm get_communicator() { return comm; };
		// this function returns the id used to register the future in the map
		unsigned int registerFuture();
		void removeFuture(unsigned int id);
		Future_Registry get_Future_Registry(unsigned int id);
};

Futures_Enviroment* Futures_Enviroment::pinstance = NULL;// initialize pointer

Futures_Enviroment* Futures_Enviroment::Instance(int &argc, char**& argv, 
	unsigned int futures_number) {

	if (!pinstance) {
		pinstance = new Futures_Enviroment(argc, argv, futures_number); // create sole instance
	}
	return pinstance; // address of sole instance
};

Futures_Enviroment* Futures_Enviroment::Instance () {

	return pinstance; // address of sole instance
};
	
Futures_Enviroment::Futures_Enviroment(int &argc, char**& argv, 
	unsigned int futures_number) {

	int mpi_status;
	MPI_Initialized(&mpi_status);
	if(!mpi_status) {
		MPI_Init(&argc, &argv);
	}
	ARMCI_Init(); //perhaps check if it is already initialized
	MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &comm);
	max_futures = futures_number;
	//Allocate shared memory
	int nprocs;
	MPI_Comm_size(comm, &nprocs);
	data = (void***)malloc(nprocs*sizeof(void**));
	ARMCI_Malloc((void **)data, futures_number*sizeof(void*));
	status = (int**)malloc(nprocs*sizeof(int*));
	ARMCI_Malloc((void **)status, futures_number*sizeof(int));
};

Futures_Enviroment::~Futures_Enviroment() {
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

unsigned int Futures_Enviroment::registerFuture() {
	unsigned int id = future_count;
	future_count++;
	assert(future_count <= max_futures);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	Future_Registry reg(data[rank][id], 1, id);
	futuresMap[id] = reg;
	return id;
}

void Futures_Enviroment::removeFuture(unsigned int id) {
	futuresMap.erase(id);
}

Future_Registry Futures_Enviroment::get_Future_Registry(unsigned int id) {
	return futuresMap[id];
}

#endif

