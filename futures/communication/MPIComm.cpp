
#include <mpi.h>
#include "communication.hpp"

#include <MPIComm.hpp>
#include <iostream>

#define GROUP_COMM_CREATE_TAG 1001

using namespace futures;
using namespace futures::communication;
//MPI_Win_Lock do not define a critical region, merely an epoch, thus we need mutexes to lock concurrent accesses
static void futures::communication::details::lock_and_get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                       int target_rank, MPI_Aint target_disp, int target_count,
                       MPI_Datatype target_datatype, MPI_Win win, MPIMutex* mutex) {
		mutex->lock(target_rank);
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
		mutex->unlock(target_rank);
};

static void futures::communication::details::lock_and_put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                          int target_rank, MPI_Aint target_disp, int target_count,
                          MPI_Datatype target_datatype, MPI_Win win, MPIMutex* mutex) {
		mutex->lock(target_rank);
		MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
		mutex->unlock(target_rank);
};

static void futures::communication::details::group_create_comm(MPI_Group group, MPI_Comm comm, MPI_Comm *comm_new, int tag) {	
	//REQUIRE: group is ordered by desired rank in comm and is identical on all callers
	int rank, grp_rank, grp_size;
	MPI_Group new_group;
	MPI_Comm_rank(comm, &rank);
	MPI_Group_rank(group, &grp_rank);
	MPI_Group_size(group, &grp_size);
	MPI_Comm_dup(MPI_COMM_SELF, comm_new);
	int *grp_pids = new int[grp_size];
	int *pids = new int[grp_size];
	for(int i=0; i < grp_size; i++) {
		grp_pids[i] = i;
	}
	MPI_Group parent_grp; 
	MPI_Comm_group(comm, &parent_grp);
	MPI_Group_translate_ranks(group, grp_size, grp_pids, parent_grp, pids);
	MPI_Group_free(&parent_grp);

	for(int merge_sz=1; merge_sz < grp_size; merge_sz = merge_sz*2) {
		int gid = grp_rank/merge_sz;
		MPI_Comm ic, comm_old = *comm_new; //FIXME: not sure if this is ok, I think openmpi has a pointer underneath
		if(gid%2 == 0) {
			if((gid+1)*merge_sz < grp_size) {
				MPI_Intercomm_create(*comm_new, 0, comm, pids[(gid+1)*merge_sz], tag, &ic);
				MPI_Intercomm_merge(ic, 0, comm_new);
			}
		}
		else {
			MPI_Intercomm_create(*comm_new , 0, comm, pids[(gid-1)*merge_sz], tag, &ic);
			MPI_Intercomm_merge(ic, 1, comm_new);
		}
		if(*comm_new != comm_old) {
			MPI_Comm_free(&ic);
			MPI_Comm_free(&comm_old);
		}
	}
	//delete pids;
	//delete grp_pids;
}

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
    status = 0;
    MPI_Win_create(&status, 1, sizeof(int), MPI_INFO_NULL, comm, &status_win);
		data_lock = new MPIMutex(comm);
		status_lock = new MPIMutex(comm);
}

MPISharedDataManager::~MPISharedDataManager() {
    MPI_Win_free(&data_win);
    MPI_Free_mem(data);
    MPI_Win_free(&status_win);
		MPI_Comm_free(&comm);
		delete data_lock;
		delete status_lock;
}

unsigned int MPISharedDataManager::get_dataSize() {
		return data_size;
};

void MPISharedDataManager::get_data(void* val) {
		//could also get current rank
		details::lock_and_get(val, data_size, datatype, dst_id, 0,
													data_size, datatype, data_win, data_lock); 
};

void MPISharedDataManager::set_data(void* val) {
		details::lock_and_put(val, data_size, datatype, dst_id, 0, 
													data_size, datatype, data_win, data_lock);
};

void MPISharedDataManager::get_status(int* val) {
	details::lock_and_get((void*)val, 1, MPI_INT, dst_id, 0, 1, MPI_INT, status_win, status_lock);
};

void MPISharedDataManager::set_status(int* val) {
	details::lock_and_put((void*)val, 1, MPI_INT, dst_id, 0, 1, MPI_INT, status_win, status_lock);
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

int MPIComm::get_procId() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
};

