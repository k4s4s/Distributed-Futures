
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <mpi.h>
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

template<typename T>
struct is_pointer { static const bool value = false; };

template<typename T>
struct is_pointer<T*> { static const bool value = true; };

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

//TODO: Dynamically allocate window (is it possible to resize?) 

/*TODO: perhaps we could have the enviroment maintain another window, that will be used to contain futures, promises, 
				 thus using RMA puts/gets to emulate shared memory model more closely. An issue here can be serialization */
class Futures_Enviroment { //singleton class
	private:
		MPI_Comm comm;
		MPI_Win data_win;
		MPI_Win status_win;
		MPI_Group group;
		void** data;
		int* status;
		//std::map<std::string, MPI_Datatype> MPI_Datatypes;
		std::map<unsigned int, Future_Registry> futuresMap; /* hold information about different futures. Attention: it is possible that
		enviroment are different accross processes */
		unsigned int max_futures; //try to get this done dynamically
		unsigned int future_count;
		static Futures_Enviroment* pinstance; 
	protected:
		Futures_Enviroment(int &argc, char**& argv, unsigned int size);
	public:
		~Futures_Enviroment();
		static Futures_Enviroment* Instance(int &argc, char**& argv, unsigned int total_futures);
		static Futures_Enviroment* Instance();
		MPI_Comm get_communicator();
	  MPI_Win get_data_window();
		MPI_Win get_status_window();
		MPI_Group get_group();
		void* get_data();
		int get_status();
		// this function returns the id used to register the future in the map
		unsigned int registerFuture();
		void removeFuture(unsigned int id);
		Future_Registry get_Future_Registry(unsigned int id);
		//MPI_Datatype get_mpi_datatype(std::string &type_name);
};

Futures_Enviroment* Futures_Enviroment::pinstance = NULL;// initialize pointer

Futures_Enviroment* Futures_Enviroment::Instance (int &argc, char**& argv, unsigned int total_futures) {
	if (!pinstance) {
		pinstance = new Futures_Enviroment(argc, argv, total_futures); // create sole instance
	}
	return pinstance; // address of sole instance
};

Futures_Enviroment* Futures_Enviroment::Instance () {

	return pinstance; // address of sole instance
};
	
Futures_Enviroment::Futures_Enviroment(int &argc, char**& argv, unsigned int total_futures) {
	int mpi_status;
	MPI_Initialized(&mpi_status);
	if(!mpi_status) {
		MPI_Init(&argc, &argv);
	}
	MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &comm);
	max_futures = total_futures;
	MPI_Alloc_mem(total_futures, MPI_INFO_NULL, &data);
	MPI_Alloc_mem(total_futures, MPI_INFO_NULL, &status);
	MPI_Alloc_mem(1, MPI_INFO_NULL, data); //FIXME:
	MPI_Win_create(data, total_futures*sizeof(void *), sizeof(void *), MPI_INFO_NULL, comm, &data_win);
	MPI_Win_create(status, total_futures*sizeof(int), sizeof(int), MPI_INFO_NULL, comm, &status_win);
};

Futures_Enviroment::~Futures_Enviroment() {
	int mpi_status;	
	//MPI_Free(status);
	//wMPI_Free();
	MPI_Win_free(&status_win);
	MPI_Win_free(&data_win);
	MPI_Comm_free(&comm);
	MPI_Finalized(&mpi_status);
	if(!mpi_status) {
		MPI_Finalize();
	}
	pinstance = NULL;
};

MPI_Comm Futures_Enviroment::get_communicator() {
	return comm;
}

MPI_Win Futures_Enviroment::get_data_window() {
	return data_win;
}

MPI_Win Futures_Enviroment::get_status_window() {
	return status_win;
}

MPI_Group Futures_Enviroment::get_group() {
	return group;
}

void* Futures_Enviroment::get_data() {
	return *data;
}

int Futures_Enviroment::get_status() {
	return *status;
}

unsigned int Futures_Enviroment::registerFuture() {
	unsigned int id = future_count;
	future_count++;	
	Future_Registry reg(*data, 1, id);
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

