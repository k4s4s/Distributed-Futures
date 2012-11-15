
#include <mpi.h>
#include "communication.hpp"

#include <MPIAsyncComm.hpp>
#include <iostream>

#define MPI_FUTURE_TAG 1000

using namespace futures;
using namespace futures::communication;
static void futures::communication::details::wait_and_recv(void *buf, int count, MPI_Datatype datatype,
    																											int source, int tag, MPI_Comm comm, 
																													MPI_Status *status, MPI_Request *request) {
		//MPI_Wait(request, status);
		MPI_Irecv(buf, count, datatype, source, tag, comm, request);
		MPI_Wait(request, status); //FIXME: breaks future::is_ready schematics, need to be nonblocking
};

static void futures::communication::details::wait_and_send(void *buf, int count, MPI_Datatype datatype, 
																													int dest,	int tag, MPI_Comm comm, 
																													MPI_Request *request, MPI_Status *status) {
    MPI_Isend(buf, count, datatype, dest, tag, comm, request);
		//MPI_Wait(request, status);
};


/*** MPIAsyncSharedDataManager impelementation ***/
MPIAsyncSharedDataManager::MPIAsyncSharedDataManager(unsigned int _data_size, unsigned int _type_size, int _id) {
    data_size = _data_size;
    type_size = _type_size;
		id = _id;
		readyFlag = 0;
};

MPIAsyncSharedDataManager::~MPIAsyncSharedDataManager() {};

unsigned int MPIAsyncSharedDataManager::get_dataSize() {
		return data_size;
};
	
void MPIAsyncSharedDataManager::get_data(void* val, int rank) {
		details::wait_and_recv(val, data_size*type_size, MPI_BYTE, rank, MPI_FUTURE_TAG+id, 
													MPI_COMM_WORLD, &data_status, &data_request);
};

void MPIAsyncSharedDataManager::set_data(void* val, int rank) {
		details::wait_and_send(val, data_size*type_size, MPI_BYTE, rank, MPI_FUTURE_TAG+id, 
													MPI_COMM_WORLD, &data_request, &data_status);
};

void MPIAsyncSharedDataManager::get_status(int* val, int rank) {
		std::cout << "getting value" << std::endl;
		details::wait_and_recv((void*)val, 1, MPI_INT, rank, MPI_FUTURE_TAG+id, 
														MPI_COMM_WORLD, &readyFlag_status, &readyFlag_request);
		std::cout << "done getting value" << std::endl;
};

void MPIAsyncSharedDataManager::set_status(int* val, int rank) {
	std::cout << "setting value" << std::endl;
	details::wait_and_send((void*)val, 1, MPI_INT, rank, MPI_FUTURE_TAG+id, 
													MPI_COMM_WORLD, &readyFlag_request, &readyFlag_status);
	std::cout << "done setting value" << std::endl;
};

/*** MPIAsyncComm implementation ***/
MPIAsyncComm::MPIAsyncComm(int &argc, char**& argv) {
    int mpi_status;
    MPI_Initialized(&mpi_status);
    if (!mpi_status) {
        MPI_Init(&argc, &argv);
    }
};

MPIAsyncComm::~MPIAsyncComm() {
    int mpi_status;
    MPI_Finalized(&mpi_status);
    if (!mpi_status) {
        MPI_Finalize();
    }
};

CommInterface* MPIAsyncComm::create(int &argc, char**& argv) {
	return new MPIAsyncComm(argc, argv);
};

SharedDataManager* MPIAsyncComm::new_sharedDataManager(unsigned int _data_size, unsigned int _type_size, int _id) {
	return new MPIAsyncSharedDataManager(_data_size, _type_size, _id);
}

int MPIAsyncComm::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

