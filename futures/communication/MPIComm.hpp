
#ifndef _MPICOMM_H
#define _MPICOMM_H

#include <mpi.h>
#include "communication.hpp"
#include "MPIMutex.hpp"

namespace futures {
namespace communication {

namespace details {
//TODO:implement actual mutexes
static void lock_and_get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                       int target_rank, MPI_Aint target_disp, int target_count,
                       MPI_Datatype target_datatype, MPI_Win win, MPIMutex* mutex);

static void lock_and_put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                          int target_rank, MPI_Aint target_disp, int target_count,
                          MPI_Datatype target_datatype, MPI_Win win, MPIMutex* mutex);

static void group_create_comm(MPI_Group group, MPI_Comm comm, MPI_Comm *comm_new, int tag);

}//end of details namespace

class MPISharedDataManager : public SharedDataManager {
private:
    MPI_Win data_win;
    MPI_Win status_win;
		MPIMutex* data_lock;
		MPIMutex* status_lock;
    void *data; //would be nice to have this as an template instead of void*
    int status;
    unsigned int data_size; //maybe we do not need this one
    unsigned int type_size;
		MPI_Comm comm;
		int src_id;
		int dst_id;
public:
    MPISharedDataManager(int _src_id, int _dst_id, 
												unsigned int _data_size, unsigned int _type_size);
    ~MPISharedDataManager();
		unsigned int get_dataSize();
    void get_data(void* val);
    void set_data(void* val);
    void get_status(int *val);
    void set_status(int *val);
};

class MPIComm : public CommInterface {
public:
    MPIComm(int &argc, char**& argv);
    ~MPIComm();
		static CommInterface* create(int &argc, char**& argv);
		SharedDataManager* new_sharedDataManager(int _src_id, int _dst_id, 
																						unsigned int _data_size, unsigned int _type_size);
    int get_procId();
};

}//end of namespace communication
}//end of namespace futures

#endif

