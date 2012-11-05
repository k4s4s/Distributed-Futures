
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <mpi.h>
#include <map>
extern "C" {
	#include <armci.h>
}


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
		void **data_buff;
		int **status_buff;
		void *data_base_ptr; //would be nice to have this as an template instead of void*
		int *status_base_ptr;
		unsigned int data_size;
		unsigned int type_size;
		int registry_initialized;
	public:
		Future_Registry(MPI_Comm comm, unsigned int _data_size, unsigned int _type_size);
		~Future_Registry();
		void **get_dataBuff();
		int **get_statusBuff();
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
	  void **get_dataBuff(unsigned int id);
		int **get_statusBuff(unsigned int id);
		// this function returns the id used to register the future in the map
		unsigned int registerFuture(unsigned int _data_size, unsigned int _type_size);
		void removeFuture(unsigned int id);
};

/*** Future_Registry impelementation ***/
Future_Registry::Future_Registry(MPI_Comm comm, unsigned int _data_size, unsigned int _type_size) {
	data_size = _data_size;
	type_size = _type_size;
	int nprocs;
	MPI_Comm_size(comm, &nprocs);
	int rank;
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	MPI_Comm_rank(env->get_communicator(), &rank);
	data_buff = (void **)malloc(nprocs*sizeof(void *));
	ARMCI_Malloc(data_buff, type_size*data_size);
	status_buff = (int **)malloc(nprocs*sizeof(int *));
	ARMCI_Malloc((void**)status_buff, sizeof(int));
	*(status_buff[rank]) = 0;
	registry_initialized = 1;
}

Future_Registry::~Future_Registry() {
	if(!registry_initialized) return;
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	ARMCI_Free(data_buff[rank]);
	free(data_buff);
	ARMCI_Free(status_buff[rank]);
	free(status_buff);			
}

void **Future_Registry::get_dataBuff() {
	return data_buff;
}

int **Future_Registry::get_statusBuff() {
	return status_buff;
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
	ARMCI_Init();
	MPI_Comm_group(MPI_COMM_WORLD, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &comm);
	total_futures = 0;
};

Futures_Enviroment::~Futures_Enviroment() {
	int mpi_status;	
	MPI_Comm_free(&comm);
	ARMCI_Finalize();
	MPI_Finalized(&mpi_status);
	if(!mpi_status) {
		MPI_Finalize();
	}
	pinstance = NULL;
};

MPI_Comm Futures_Enviroment::get_communicator() {
	return comm;
}

void **Futures_Enviroment::get_dataBuff(unsigned int id) {
	Future_Registry *reg = futuresMap[id];
	return reg->get_dataBuff();
};

int **Futures_Enviroment::get_statusBuff(unsigned int id) {
	Future_Registry *reg = futuresMap[id];
	return reg->get_statusBuff();
};

unsigned int Futures_Enviroment::registerFuture(unsigned int _data_size, unsigned int _type_size) {
	unsigned int id = total_futures;
	total_futures++;
	futuresMap[id] = new Future_Registry(comm, _data_size, _type_size);
	return id;
};

void Futures_Enviroment::removeFuture(unsigned int id) {
	Future_Registry *reg = futuresMap[id];
	futuresMap.erase(id);
	delete reg;
};

#endif

