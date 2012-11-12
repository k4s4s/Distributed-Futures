
#ifndef _MPICOMM_H
#define _MPICOMM_H

#include <mpi.h>
#include "communication.hpp"

#include <iostream>

namespace futures {
namespace communication {

namespace details {
//TODO:implement actual mutexes
static void lock_and_get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                       int target_rank, MPI_Aint target_disp, int target_count,
                       MPI_Datatype target_datatype, MPI_Win win, int locktype) {
    MPI_Win_lock(locktype, target_rank, 0, win);
    MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
};

static void lock_and_put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                          int target_rank, MPI_Aint target_disp, int target_count,
                          MPI_Datatype target_datatype, MPI_Win win, int locktype) {
    MPI_Win_lock(locktype, target_rank, 0, win);
    MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
};

}//end of details namespace

class MPISharedDataManager : public SharedDataManager {
private:
    MPI_Win data_win;
    MPI_Win status_win;
    void *data; //would be nice to have this as an template instead of void*
    int status;
    unsigned int data_size; //maybe we do not need this one
    unsigned int type_size;
public:
    MPISharedDataManager(unsigned int _data_size, unsigned int _type_size);
    ~MPISharedDataManager();
		unsigned int get_dataSize();
    void get_data(void* val);
    void set_data(void* val, int rank);
    void get_status(int *val);
    void set_status(int *val, int rank);
};

class MPIComm : public CommInterface {
public:
    MPIComm(int &argc, char**& argv);
    ~MPIComm();
		static CommInterface* create(int &argc, char**& argv);
		SharedDataManager* new_sharedDataManager(unsigned int _data_size, unsigned int _type_size);
    int get_procId();
};

/*** MPISharedDataManager impelementation ***/
MPISharedDataManager::MPISharedDataManager(unsigned int _data_size, unsigned int _type_size) { //TODO: workers should not allcate any memory
    data_size = _data_size;
    type_size = _type_size;
    MPI_Alloc_mem(type_size*data_size, MPI_INFO_NULL, &data);
    MPI_Win_create(data, data_size, type_size, MPI_INFO_NULL, MPI_COMM_WORLD, &data_win);
    status = 0;
    MPI_Win_create(&status, 1, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &status_win);
}

MPISharedDataManager::~MPISharedDataManager() {
    MPI_Win_free(&data_win);
    MPI_Free_mem(data);
    MPI_Win_free(&status_win);
}

unsigned int MPISharedDataManager::get_dataSize() {
		return data_size;
};
	
void MPISharedDataManager::get_data(void* val) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		details::lock_and_get(val, data_size*type_size, MPI_BYTE, rank, 0, 
													data_size*type_size, MPI_BYTE, data_win, MPI_LOCK_EXCLUSIVE);
};

void MPISharedDataManager::set_data(void* val, int rank) {
		details::lock_and_put(val, data_size*type_size, MPI_BYTE, rank, 0, 
													data_size*type_size, MPI_BYTE, data_win, MPI_LOCK_EXCLUSIVE);
};

void MPISharedDataManager::get_status(int* val) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	details::lock_and_get((void*)val, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win, MPI_LOCK_SHARED);
};

void MPISharedDataManager::set_status(int* val, int rank) {
	details::lock_and_put((void*)val, 1, MPI_INT, rank, 0, 1, MPI_INT, status_win, MPI_LOCK_SHARED);
};

/*** MPIComm implementation ***/
MPIComm::MPIComm(int &argc, char**& argv) {
    int mpi_status;
    MPI_Initialized(&mpi_status);
    if (!mpi_status) {
        MPI_Init(&argc, &argv);
    }
};

MPIComm::~MPIComm() {
    int mpi_status;
    MPI_Finalized(&mpi_status);
    if (!mpi_status) {
        MPI_Finalize();
    }
};

CommInterface* MPIComm::create(int &argc, char**& argv) {
	return new MPIComm(argc, argv);
};

SharedDataManager* MPIComm::new_sharedDataManager(unsigned int _data_size, unsigned int _type_size) {
	return new MPISharedDataManager(_data_size, _type_size);
}

int MPIComm::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

}//end of namespace communication
}//end of namespace futures

#endif

