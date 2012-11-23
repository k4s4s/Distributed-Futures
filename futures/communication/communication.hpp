
#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <boost/mpi/datatype.hpp>
// For (de-)serializing sends and receives
#include <boost/mpi/packed_oarchive.hpp>
#include <boost/mpi/packed_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <mpi.h>

namespace futures {
namespace communication {

class SharedDataManager {
public:
    virtual ~SharedDataManager() {};
		virtual unsigned int get_dataSize() = 0;
    virtual void get_data(void* val) = 0;
		virtual void get_data(boost::mpi::packed_iarchive& ar) = 0;
    virtual void set_data(void* val) = 0;
		virtual void set_data(boost::mpi::packed_oarchive& ar) = 0;
    virtual void get_status(int* val) = 0;
    virtual void set_status(int* val) = 0;
		virtual MPI_Comm get_comm() = 0;
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

