
#ifndef _ARMCIComm_H
#define _ARMCIComm_H

#include <mpi.h>
extern "C" {
	#include <armci.h>
}
#include "communication.hpp"
#include <iostream>

#ifdef ARMCI_MPI_V
	#define ARMCI_Access_begin(buff) ARMCI_Access_begin(buff)
	#define ARMCI_Access_end(buff) ARMCI_Access_end(buff)
#else
	#define ARMCI_Access_begin(buff)
	#define ARMCI_Access_end(buff)
#endif

namespace futures {
namespace communication {

namespace details {
//TODO:add locks...
static void lock_and_get(void *origin_addr, void** data_buff, int data_size, int rank) {
				ARMCI_Access_begin(data_buff);
				memcpy (origin_addr, data_buff[rank], data_size);
				//origin_addr = data_buff[rank];
				ARMCI_Access_end(data_buff);
};
//TODO: add locks here too
static void lock_and_put(void *origin_addr, void** data_buff, int data_size, int target_rank) {
				ARMCI_Put(origin_addr, data_buff[target_rank], data_size, target_rank);
};

}//end of details namespace

class ARMCISharedDataManager : public SharedDataManager {
private:
		void **data_buff;
		int **status_buff;
		unsigned int data_size;
		unsigned int type_size;
public:
    ARMCISharedDataManager(unsigned int _data_size, unsigned int _type_size);
    ~ARMCISharedDataManager();
		unsigned int get_dataSize();
    void get_data(void* val);
    void set_data(void* val, int rank);
    void get_status(int *val);
    void set_status(int *val, int rank);
};

class ARMCIComm : public CommInterface {
public:
    ARMCIComm(int &argc, char**& argv);
    ~ARMCIComm();
		static CommInterface* create(int &argc, char**& argv);
		SharedDataManager* new_sharedDataManager(unsigned int _data_size, unsigned int _type_size);
    int get_procId();
};

/*** ARMCISharedDataManager impelementation ***/
ARMCISharedDataManager::ARMCISharedDataManager(unsigned int _data_size, unsigned int _type_size) { //TODO: workers should not allcate any memory
	data_size = _data_size;
	type_size = _type_size;
	int nprocs;
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	data_buff = (void **)malloc(nprocs*sizeof(void *));
	ARMCI_Malloc(data_buff, type_size*data_size);
	status_buff = (int **)malloc(nprocs*sizeof(int *));
	ARMCI_Malloc((void**)status_buff, sizeof(int));
	*status_buff[rank] = 0;
}

ARMCISharedDataManager::~ARMCISharedDataManager() {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	ARMCI_Free(data_buff[rank]);
	free(data_buff);
	ARMCI_Free(status_buff[rank]);
	free(status_buff);	
}

unsigned int ARMCISharedDataManager::get_dataSize() {
		return data_size;
};
	
void ARMCISharedDataManager::get_data(void* val) {
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		details::lock_and_get(val, data_buff, data_size*type_size, rank);
};

void ARMCISharedDataManager::set_data(void* val, int rank) {
		details::lock_and_put(val, data_buff, data_size*type_size, rank);
};

void ARMCISharedDataManager::get_status(int* val) {
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		details::lock_and_get((void*)val, (void**)status_buff, sizeof(int), rank);
};

void ARMCISharedDataManager::set_status(int* val, int rank) {
		details::lock_and_put((void*)val, (void**)status_buff, sizeof(int), rank);
};

/*** ARMCIComm implementation ***/
ARMCIComm::ARMCIComm(int &argc, char**& argv) {
	int mpi_status;
	MPI_Initialized(&mpi_status);
	if(!mpi_status) {
		MPI_Init(&argc, &argv);
	}
	ARMCI_Init();
};

ARMCIComm::~ARMCIComm() {
	int mpi_status;	
	ARMCI_Finalize();
	MPI_Finalized(&mpi_status);
	if(!mpi_status) {
		MPI_Finalize();
	}
};

CommInterface* ARMCIComm::create(int &argc, char**& argv) {
	return new ARMCIComm(argc, argv);
};

SharedDataManager* ARMCIComm::new_sharedDataManager(unsigned int _data_size, unsigned int _type_size) {
	return new ARMCISharedDataManager(_data_size, _type_size);
};

int ARMCIComm::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

}//end of namespace communication
}//end of namespace futures

#endif

