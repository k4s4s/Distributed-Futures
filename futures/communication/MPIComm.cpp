
#include "communication.hpp"
#include "MPIComm.hpp"
#include <mpi.h>
#include <boost/mpi.hpp>
#include <iostream>
#include "../common.hpp"
#include "mpi_details.hpp"

#define GROUP_COMM_CREATE_TAG 1001
#define NEW_JOB	2001

using namespace futures;
using namespace futures::communication;

/*** MPISharedDataManager impelementation ***/
MPISharedDataManager::MPISharedDataManager(int _src_id, int _dst_id,
        unsigned int _data_size, unsigned int _type_size,
        MPI_Datatype _datatype) {
    int pids[2];
    src_id = _src_id;
    dst_id = _dst_id;
    //need to assign them in ordered fashion
    pids[0] = (src_id>dst_id)?src_id:dst_id;
    pids[1] = (src_id<dst_id)?src_id:dst_id;
    MPI_Group newgroup, worldgroup;
    MPI_Comm_group(MPI_COMM_WORLD, &worldgroup);
    MPI_Group_incl(worldgroup, 2, pids,	&newgroup);
    details::group_create_comm(newgroup, MPI_COMM_WORLD, &comm, GROUP_COMM_CREATE_TAG);
    data_size = _data_size;
    type_size = _type_size;
    datatype = _datatype;
    MPI_Alloc_mem(type_size*data_size, MPI_INFO_NULL, &data);
    MPI_Win_create(data, data_size, type_size, MPI_INFO_NULL, comm, &data_win);
    ar_size = 0;
    MPI_Win_create(&ar_size, 1, sizeof(int), MPI_INFO_NULL, comm, &ar_size_win);
    status = 0;
    MPI_Win_create(&status, 1, sizeof(int), MPI_INFO_NULL, comm, &status_win);
    data_lock = new MPIMutex(comm);
    ar_size_lock = new MPIMutex(comm);
    status_lock = new MPIMutex(comm);
}

MPISharedDataManager::~MPISharedDataManager() {
    MPI_Win_free(&data_win);
    MPI_Free_mem(data);
    MPI_Win_free(&ar_size_win);
    MPI_Win_free(&status_win);
    MPI_Comm_free(&comm);
    delete data_lock;
    delete ar_size_lock;
    delete status_lock;
}

unsigned int MPISharedDataManager::get_dataSize() {
    return data_size;
};

void MPISharedDataManager::get_data(void* val) {
    details::lock_and_get(val, data_size, datatype, dst_id, 0,
                          data_size, datatype, data_win, data_lock);
};

void MPISharedDataManager::get_data(boost::mpi::packed_iarchive& ar) {
    int count;
    details::lock_and_get((void*)&count, 1, MPI_INT, dst_id, 0,
                          1, MPI_INT, ar_size_win, ar_size_lock);
    // Prepare input buffer and receive the message
    ar.resize(count);
    details::lock_and_get(const_cast<void*>(ar.address()), ar.size(), MPI_PACKED, dst_id,
                          0, ar.size(), MPI_PACKED, data_win, data_lock);
};

void MPISharedDataManager::set_data(void* val) {
    details::lock_and_put(val, data_size, datatype, dst_id, 0,
                          data_size, datatype, data_win, data_lock);
};

void MPISharedDataManager::set_data(boost::mpi::packed_oarchive& ar) {
    details::lock_and_put((void*)(&ar.size()), 1, MPI_INT, dst_id, 0,
                          1, MPI_INT, ar_size_win, ar_size_lock);
    details::lock_and_put(const_cast<void*>(ar.address()), ar.size(), MPI_PACKED, dst_id, 0,
                          ar.size(), MPI_PACKED, data_win, data_lock);
};

void MPISharedDataManager::get_status(int* val) {
    details::lock_and_get((void*)val, 1, MPI_INT, dst_id, 0, 1, MPI_INT, status_win, status_lock);
};

void MPISharedDataManager::set_status(int* val) {
    details::lock_and_put((void*)val, 1, MPI_INT, dst_id, 0, 1, MPI_INT, status_win, status_lock);
};

MPI_Comm MPISharedDataManager::get_comm() {
    return comm;
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

SharedDataManager* MPIComm::new_sharedDataManager(int _src_id, int _dst_id,
        unsigned int _data_size, unsigned int _type_size,
        MPI_Datatype _datatype) {
    return new MPISharedDataManager(_src_id, _dst_id, _data_size, _type_size, _datatype);
}

void MPIComm::send(int dst_id, int tag, int count, MPI_Datatype datatype, void* data) {
    MPI_Send(data, count, datatype, dst_id, tag, MPI_COMM_WORLD);
};

void MPIComm::send(int dst_id, int tag, boost::mpi::packed_oarchive& ar) {
    MPI_Send((void*)(&ar.size()), 1, MPI_INT, dst_id, tag, MPI_COMM_WORLD);
    MPI_Send(const_cast<void*>(ar.address()), ar.size(), MPI_PACKED, dst_id, tag+1, MPI_COMM_WORLD);
};

void MPIComm::recv(int src_id, int tag, int count, MPI_Datatype datatype, void* data) {
    MPI_Status status;
    MPI_Recv(data, count, datatype, src_id, tag, MPI_COMM_WORLD, &status);
};

void MPIComm::recv(int src_id, int tag, boost::mpi::packed_iarchive& ar) {
    MPI_Status status;
    int count;
    MPI_Recv((void*)&count, 1, MPI_INT, src_id, tag, MPI_COMM_WORLD, &status);
    ar.resize(count);
    MPI_Recv(const_cast<void*>(ar.address()), count, MPI_PACKED, src_id, tag+1,
             MPI_COMM_WORLD, &status);
};

int MPIComm::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

int MPIComm::size() {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return size;
};

