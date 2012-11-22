
#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <mpi.h>

namespace futures {
namespace communication {

class SharedDataManager {
public:
    virtual ~SharedDataManager() {};
		virtual unsigned int get_dataSize() = 0;
    virtual void get_data(void* val) = 0;
    virtual void set_data(void* val) = 0;
    virtual void get_status(int* val) = 0;
    virtual void set_status(int* val) = 0;
};

class CommInterface {
public:
    //FIXME: need to pass type somehow to SharedDataManager
    virtual ~CommInterface() {};
		//User should also implement a create function for the CommInterface factory
		virtual SharedDataManager* new_sharedDataManager(int src_id, int dst_id, 
																										unsigned int _data_size, unsigned int _type_size,
																										MPI_Datatype _datatype) = 0;
    virtual int get_procId() = 0;
};

}//end of namespace communication
}//end of namespace futures

#endif

