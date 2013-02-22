
#include "MPIMutex.hpp"
#include "MPIComm.hpp"
#include "mpi_details.hpp"
#include "../common.hpp"
#include <cassert>

#define GROUP_COMM_CREATE_TAG 1001

#define AR_SIZE_OFFSET 0
#define DATA_OFFSET sizeof(int)

using namespace futures;
using namespace futures::communication;

/*** MPI_Shared_address_space implementation ***/
MPI_Shared_address_space::MPI_Shared_address_space(std::size_t size) {
	
	MPI_Alloc_mem(size, MPI_INFO_NULL, &shared_memory);
  MPI_Win_create(shared_memory, size, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &shared_memory_win);
};

MPI_Shared_address_space::~MPI_Shared_address_space() {

	MPI_Win_free(&shared_memory_win);
  MPI_Free_mem(shared_memory);
};

void MPI_Shared_address_space::put(void* data, int dst_id, int count, int offset, 
																	MPI_Datatype datatype) {
    communication::details::MPI_Exclusive_put(data, count, datatype, dst_id, 
																							offset,
                          										count, datatype, shared_memory_win);
};

void MPI_Shared_address_space::put(boost::mpi::packed_oarchive& ar, int dst_id, int offset) {
    communication::details::MPI_Exclusive_put((void*)(&ar.size()), 1, MPI_INT, dst_id, 
																							offset+AR_SIZE_OFFSET,
                          										1, MPI_INT, shared_memory_win);
    communication::details::MPI_Exclusive_put(const_cast<void*>(ar.address()), ar.size(), 
																							MPI_PACKED, dst_id,
																							offset+DATA_OFFSET, ar.size(), 
																							MPI_PACKED, shared_memory_win);
};

void MPI_Shared_address_space::get(void* data, int src_id, int count, int offset, MPI_Datatype datatype) {
    communication::details::MPI_Exclusive_get(data, count, datatype, src_id, 
																							offset,
                          										count, datatype, shared_memory_win);
};

void MPI_Shared_address_space::get(boost::mpi::packed_iarchive& ar, int src_id, int offset) {
		int count;
    communication::details::MPI_Exclusive_get((void*)&count, 1, MPI_INT, src_id, 
																							offset+AR_SIZE_OFFSET,
		                          								1, MPI_INT, shared_memory_win);
    // Prepare input buffer and receive the message
    ar.resize(count);
    communication::details::MPI_Exclusive_get(const_cast<void*>(ar.address()), ar.size(), 
																							MPI_PACKED, src_id,	offset+DATA_OFFSET,
																							ar.size(), MPI_PACKED, shared_memory_win);
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

mutex* MPIComm::new_lock() {
	return new MPIMutex(MPI_COMM_WORLD);
};

Shared_Address_space* MPIComm::new_shared_space(std::size_t size) {
	return new MPI_Shared_address_space(size);
};

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

