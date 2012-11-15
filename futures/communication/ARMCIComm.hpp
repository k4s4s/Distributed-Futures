
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
public:
    ARMCISharedDataManager(unsigned int _data_size, unsigned int _type_size, int _id);
    ~ARMCISharedDataManager();
		unsigned int get_dataSize();
    void get_data(void* val, int rank);
    void set_data(void* val, int rank);
    void get_status(int *val, int rank);
    void set_status(int *val, int rank);
};

class ARMCIComm : public CommInterface {
public:
    ARMCIComm(int &argc, char**& argv);
    ~ARMCIComm();
		static CommInterface* create(int &argc, char**& argv);
		SharedDataManager* new_sharedDataManager(unsigned int _data_size, unsigned int _type_size, int _id);
    int get_procId();
};

}//end of namespace communication
}//end of namespace futures

#endif

