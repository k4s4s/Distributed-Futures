
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

//TODO: have only the master proc each time share its data, workers should use NULL when creating windows
class Future_Registry {
	private:
		MPI_Win data_win;
		MPI_Win status_win;
		void *data; //would be nice to have this as an template instead of void*
		int status;
		unsigned int data_size; //maybe we do not need this one
		unsigned int type_size;
		bool registry_initialized;
	public:
		Future_Registry(MPI_Comm comm, unsigned int _data_size, unsigned int _type_size);
		~Future_Registry();
		MPI_Win get_dataWin();
		MPI_Win get_statusWin();
		unsigned int get_dataSize();
};

/*TODO: perhaps we could have the enviroment maintain another window, that will be used to contain futures, promises, 
				 thus using RMA puts/gets to emulate shared memory model more closely. An issue here can be serialization */
class Futures_Enviroment { //singleton class
	private:
		MPI_Comm comm;
		MPI_Group group;
		static std::map<unsigned int, Future_Registry*> futuresMap; /* hold information about different futures. Attention: it is possible that
		enviroment are different accross processes */
		static unsigned int total_futures; //never decrease that value, it is  not actual number of futures, just next avaible id for the map
		static Futures_Enviroment* pinstance; 
	protected:
		Futures_Enviroment(int &argc, char**& argv);
	public:
		~Futures_Enviroment();
		static Futures_Enviroment* Instance(int &argc, char**& argv);
		static Futures_Enviroment* Instance();
		MPI_Comm get_communicator();
	  MPI_Win get_dataWindow(unsigned int id);
		MPI_Win get_statusWindow(unsigned int id);
		MPI_Group get_group();
		// this function returns the id used to register the future in the map
		unsigned int registerFuture(unsigned int _data_size, unsigned int _type_size);
		void removeFuture(unsigned int id);
};

/*** Future_Registry impelementation ***/
Future_Registry::Future_Registry(MPI_Comm comm, unsigned int _data_size, unsigned int _type_size) {
	data_size = _data_size;
	type_size = _type_size;
	MPI_Alloc_mem(type_size*data_size, MPI_INFO_NULL, &data);
	MPI_Win_create(data, data_size, type_size, MPI_INFO_NULL, comm, &data_win);
	status = 0;
	MPI_Win_create(&status, 1, sizeof(int), MPI_INFO_NULL, comm, &status_win);
	registry_initialized = true;
}

Future_Registry::~Future_Registry() {
	if(!registry_initialized) return;
	MPI_Win_free(&data_win);	
	MPI_Free_mem(data);
	MPI_Win_free(&status_win);
}

MPI_Win Future_Registry::get_dataWin() {
	return data_win;
}

MPI_Win Future_Registry::get_statusWin() {
	return status_win;
}

unsigned int Future_Registry::get_dataSize() {
	return data_size;
}

/*** Future_Enviroment impelementation ***/
std::map<unsigned int, Future_Registry*> Futures_Enviroment::futuresMap;
unsigned int Futures_Enviroment::total_futures = 0;
Futures_Enviroment* Futures_Enviroment::pinstance = NULL;// initialize pointer

Futures_Enviroment* Futures_Enviroment::Instance (int &argc, char**& argv) {
	if (!pinstance) {
		pinstance = new Futures_Enviroment(argc, argv); // create sole instance
	}
	return pinstance; // address of sole instance
};

Futures_Enviroment* Futures_Enviroment::Instance () {

	return pinstance; // address of sole instance
};

Futures_Enviroment::Futures_Enviroment(int &argc, char**& argv) {
	int mpi_status;
	MPI_Initialized(&mpi_status);
	if(!mpi_status) {
		MPI_Init(&argc, &argv);
	}
	MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &comm);
	total_futures = 0;
};

Futures_Enviroment::~Futures_Enviroment() {
	int mpi_status;	
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

MPI_Win Futures_Enviroment::get_dataWindow(unsigned int id) {
	Future_Registry *reg = futuresMap[id];
	return reg->get_dataWin();
}

MPI_Win Futures_Enviroment::get_statusWindow(unsigned int id) {
	Future_Registry *reg = futuresMap[id];
	return reg->get_statusWin();
}

unsigned int Futures_Enviroment::registerFuture(unsigned int _data_size, unsigned int _type_size) {
	unsigned int id = total_futures;
	total_futures++;
	futuresMap[id] = new Future_Registry(comm, _data_size, _type_size);
	return id;
}

void Futures_Enviroment::removeFuture(unsigned int id) {
	Future_Registry *reg = futuresMap[id];
	futuresMap.erase(id);
	delete reg;
}

#endif

