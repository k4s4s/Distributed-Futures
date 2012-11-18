
#ifndef _ARMCIComm_H
#define _ARMCIComm_H

#include <mpi.h>
extern "C" {
	#include <armci.h>
}
#include "communication.hpp"

namespace futures {
namespace communication {

enum locktype {
	DATA_LOCK = 0,
	STATUS_LOCK = 1
};

namespace details {
//TODO:add locks...
static void lock_and_get(void *origin_addr, void** data_buff, int data_size, 
												int rank, locktype lockt);
//TODO: add locks here too
static void lock_and_put(void *origin_addr, void** data_buff, int data_size, 
												int target_rank, locktype lockt);

}//end of details namespace

class ARMCISharedDataManager : public SharedDataManager {
private:
		void **data_buff;
		int **status_buff;
		unsigned int data_size;
		unsigned int type_size;
		int src_id;
		int dst_id;
public:
    ARMCISharedDataManager(int _scr_id, int _dst_id, 
													unsigned int _data_size, unsigned int _type_size);
    ~ARMCISharedDataManager();
		unsigned int get_dataSize();
    void get_data(void* val);
    void set_data(void* val);
    void get_status(int *val);
    void set_status(int *val);
};

class ARMCIComm : public CommInterface {
public:
    ARMCIComm(int &argc, char**& argv);
    ~ARMCIComm();
		static CommInterface* create(int &argc, char**& argv);
		SharedDataManager* new_sharedDataManager(int _src_id, int _dst_id, 
																						unsigned int _data_size, unsigned int _type_size);
    int get_procId();
};

}//end of namespace communication
}//end of namespace futures

#endif

