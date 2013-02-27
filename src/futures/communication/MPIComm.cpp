
#include "MPIMutex.hpp"
#include "MPIComm.hpp"
#include "mpi_details.hpp"
#include "../common.hpp"
#include <cassert>

#define GROUP_COMM_CREATE_TAG 1001

using namespace futures;
using namespace futures::communication;

/*** MPI_Shared_Address_space implementation ***/
Shared_Address_space::Shared_Address_space(std::size_t size) {
	
	MPI_Alloc_mem(size, MPI_INFO_NULL, &shared_memory);
  MPI_Win_create(shared_memory, size, 1, MPI_INFO_NULL, 
								MPI_COMM_WORLD, &shared_memory_win);
};

Shared_Address_space::~Shared_Address_space() {

	MPI_Win_free(&shared_memory_win);
  MPI_Free_mem(shared_memory);
};

/*** CommInterface implementation ***/
CommInterface::CommInterface(int &argc, char**& argv) {
    int mpi_status;
    MPI_Initialized(&mpi_status);
    if (!mpi_status) {
        MPI_Init(&argc, &argv);
    }
};

CommInterface::~CommInterface() {
    int mpi_status;
    MPI_Finalized(&mpi_status);
    if (!mpi_status) {
        MPI_Finalize();
    }
};

mutex* CommInterface::new_lock() {
	return new MPIMutex(MPI_COMM_WORLD);
};

Shared_Address_space* CommInterface::new_shared_space(std::size_t size) {
	return new Shared_Address_space(size);
};

void CommInterface::send(int dst_id, int tag, int count, MPI_Datatype datatype, void* data) {
    MPI_Send(data, count, datatype, dst_id, tag, MPI_COMM_WORLD);
};

void CommInterface::send(int dst_id, int tag, boost::mpi::packed_oarchive& ar) {
    MPI_Send((void*)(&ar.size()), 1, MPI_INT, dst_id, tag, MPI_COMM_WORLD);
    MPI_Send(const_cast<void*>(ar.address()), ar.size(), MPI_PACKED, dst_id, tag+1, MPI_COMM_WORLD);
};

void CommInterface::recv(int src_id, int tag, int count, MPI_Datatype datatype, void* data) {
    MPI_Status status;
    MPI_Recv(data, count, datatype, src_id, tag, MPI_COMM_WORLD, &status);
};

void CommInterface::recv(int src_id, int tag, boost::mpi::packed_iarchive& ar) {
    MPI_Status status;
    int count;
    MPI_Recv((void*)&count, 1, MPI_INT, src_id, tag, MPI_COMM_WORLD, &status);
    ar.resize(count);
    MPI_Recv(const_cast<void*>(ar.address()), count, MPI_PACKED, src_id, tag+1,
             MPI_COMM_WORLD, &status);
};

int CommInterface::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

int CommInterface::size() {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return size;
};

