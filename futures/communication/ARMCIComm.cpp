
#include "ARMCIComm.hpp"
#include <iostream>

#ifdef ARMCI_MPI_V
	#define ARMCI_Access_begin(buff) ARMCI_Access_begin(buff)
	#define ARMCI_Access_end(buff) ARMCI_Access_end(buff)
#else
	#define ARMCI_Access_begin(buff)
	#define ARMCI_Access_end(buff)
#endif

static void futures::communication::details::lock_and_get(void *origin_addr, 
																													void** data_buff, int data_size, 
																													int rank, locktype lockt) {
				ARMCI_Lock(lockt, rank);
				ARMCI_Access_begin(data_buff[rank]);
				memcpy (origin_addr, data_buff[rank], data_size);
				//origin_addr = data_buff[rank];
				ARMCI_Access_end(data_buff[rank]);
				ARMCI_Unlock(lockt, rank);
};
//TODO: add locks here too
static void futures::communication::details::lock_and_put(void *origin_addr,
																													void** data_buff, int data_size, 
																													int target_rank, locktype lockt) {
				ARMCI_Lock(lockt, target_rank);
				ARMCI_Put(origin_addr, data_buff[target_rank], data_size, target_rank);
				ARMCI_Unlock(lockt, target_rank);
};

using namespace futures;
using namespace futures::communication;

/*** ARMCISharedDataManager impelementation ***/
ARMCISharedDataManager::ARMCISharedDataManager(int _src_id, int _dst_id,
																							unsigned int _data_size, unsigned int _type_size) {
	data_size = _data_size;
	type_size = _type_size;
	src_id = _src_id;
	dst_id = _dst_id;
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
	free(status_buff);	//FIXME: WE need to free all the shared space
}

unsigned int ARMCISharedDataManager::get_dataSize() {
		return data_size;
};
	
void ARMCISharedDataManager::get_data(void* val) {
		details::lock_and_get(val, data_buff, data_size*type_size, dst_id, DATA_LOCK);
};

void ARMCISharedDataManager::set_data(void* val) {
		details::lock_and_put(val, data_buff, data_size*type_size, dst_id, DATA_LOCK);
};

void ARMCISharedDataManager::get_status(int* val) {
		details::lock_and_get((void*)val, (void**)status_buff, sizeof(int), dst_id, STATUS_LOCK);
		//std::cout << "val:" << *val << std::endl; 
};

void ARMCISharedDataManager::set_status(int* val) {
//std::cout << "val:" << *val << std::endl;
		details::lock_and_put((void*)val, (void**)status_buff, sizeof(int), dst_id, STATUS_LOCK);
//std::cout << "val:" << *val << std::endl;
};

/*** ARMCIComm implementation ***/
ARMCIComm::ARMCIComm(int &argc, char**& argv) {
	int mpi_status;
	MPI_Initialized(&mpi_status);
	if(!mpi_status) {
		MPI_Init(&argc, &argv);
	}
	ARMCI_Init();
	ARMCI_Create_mutexes(2);
};

ARMCIComm::~ARMCIComm() {
	int mpi_status;	
	ARMCI_Destroy_mutexes();
	ARMCI_Finalize();
	MPI_Finalized(&mpi_status);
	if(!mpi_status) {
		MPI_Finalize();
	}
};

CommInterface* ARMCIComm::create(int &argc, char**& argv) {
	return new ARMCIComm(argc, argv);
};

SharedDataManager* ARMCIComm::new_sharedDataManager(int _src_id, int _dst_id, 
																										unsigned int _data_size, unsigned int _type_size) {
	return new ARMCISharedDataManager(_src_id, _dst_id, _data_size, _type_size);
};

int ARMCIComm::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

