
#include "communication.hpp"
#include "MPIComm.hpp"
#include <mpi.h>
#include <boost/mpi.hpp>
#include <iostream>
#include "../common.hpp"
#include "mpi_details.hpp"
#include <cassert>

#define GROUP_COMM_CREATE_TAG 1001
#define NEW_JOB	2001

using namespace futures;
using namespace futures::communication;

/*** MPI_Shared_data impelementation ***/
MPI_Shared_data::MPI_Shared_data(int _dst_id, Shared_pointer _ptr,
                    unsigned int _data_size, unsigned int _type_size,
                    MPI_Datatype _datatype, MPI_Win _data_win, MPIMutex* _data_lock) {
	  dst_id = _dst_id;
		ptr = _ptr;
	  data_size = _data_size;
	  type_size = _type_size;
	  datatype = _datatype;
		data_win = _data_win;
		data_lock = _data_lock;
	  ar_size = 0;
	  status = 0;
		comm = MPI_COMM_WORLD;
};

MPI_Shared_data::~MPI_Shared_data() {};

unsigned int MPI_Shared_data::get_dataSize() {
    return data_size;
};

void MPI_Shared_data::get_data(void* val) {
    details::lock_and_get(val, data_size, datatype, dst_id, ptr.base_address+DATA_OFFSET,
                          data_size, datatype, data_win, data_lock);
};

void MPI_Shared_data::get_data(boost::mpi::packed_iarchive& ar) {
    int count;
    details::lock_and_get((void*)&count, 1, MPI_INT, dst_id, ptr.base_address+AR_SIZE_OFFSET,
                          1, MPI_INT, data_win, data_lock);
    // Prepare input buffer and receive the message
    ar.resize(count);
    details::lock_and_get(const_cast<void*>(ar.address()), ar.size(), MPI_PACKED, dst_id,
                          ptr.base_address+DATA_OFFSET, ar.size(), MPI_PACKED, data_win, data_lock);
};

void MPI_Shared_data::set_data(void* val) {
    details::lock_and_put(val, data_size, datatype, dst_id, ptr.base_address+DATA_OFFSET,
                          data_size, datatype, data_win, data_lock);
};

void MPI_Shared_data::set_data(boost::mpi::packed_oarchive& ar) {
    details::lock_and_put((void*)(&ar.size()), 1, MPI_INT, dst_id, ptr.base_address+AR_SIZE_OFFSET,
                          1, MPI_INT, data_win, data_lock);
    details::lock_and_put(const_cast<void*>(ar.address()), ar.size(), MPI_PACKED, dst_id, 
													ptr.base_address+DATA_OFFSET, ar.size(), MPI_PACKED, data_win, data_lock);
};

void MPI_Shared_data::get_status(int* val) {
    details::lock_and_get((void*)val, 1, MPI_INT, dst_id, ptr.base_address+STATUS_OFFSET, 1, 
													MPI_INT, data_win, data_lock);
};

void MPI_Shared_data::set_status(int* val) {
    details::lock_and_put((void*)val, 1, MPI_INT, dst_id, ptr.base_address+STATUS_OFFSET, 1, 
													MPI_INT, data_win, data_lock);
};

MPI_Comm MPI_Shared_data::get_comm() {
    return comm;
};

Shared_pointer MPI_Shared_data::get_shared_pointer() {
	return ptr;
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

Shared_data* MPIComm::new_Shared_data(int _dst_id,
																			Shared_pointer _ptr,
                    									unsigned int _data_size, unsigned int _type_size,
                    									MPI_Datatype _datatype, MPI_Win _data_win, MPIMutex* _data_lock) {
    return new MPI_Shared_data(_dst_id, _ptr, 
															_data_size, _type_size, _datatype,
															_data_win, _data_lock);
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

