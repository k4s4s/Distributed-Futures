
#ifndef _MPI_DETAILS_H
#define _MPI_DETAILS_H

#include <mpi.h>
#include "MPIMutex.hpp"

namespace futures {
namespace communication {
namespace details {
//TODO:implement actual mutexes
static void MPI_Exclusive_get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                         int target_rank, MPI_Aint target_disp, int target_count,
                         MPI_Datatype target_datatype, MPI_Win win);

static void MPI_Exclusive_put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                         int target_rank, MPI_Aint target_disp, int target_count,
                         MPI_Datatype target_datatype, MPI_Win win);

static void MPI_Group_create_comm(MPI_Group group, MPI_Comm comm, MPI_Comm *comm_new, int tag);

}//end of details namespace
}
}

static void futures::communication::details::MPI_Exclusive_get(void *origin_addr,
        int origin_count, MPI_Datatype origin_datatype,
        int target_rank, MPI_Aint target_disp, int target_count,
        MPI_Datatype target_datatype, MPI_Win win) {
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
};

static void futures::communication::details::MPI_Exclusive_put(void *origin_addr,
        int origin_count, MPI_Datatype origin_datatype,
        int target_rank, MPI_Aint target_disp, int target_count,
        MPI_Datatype target_datatype, MPI_Win win) {
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
};

static void futures::communication::details::MPI_Group_create_comm(MPI_Group group,
        MPI_Comm comm, MPI_Comm *comm_new, int tag) {
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
    delete pids;
    delete grp_pids;
};

#endif

