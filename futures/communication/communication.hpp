
#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <mpi.h>
#include <boost/mpi.hpp>
#include <mutex.hpp>

namespace futures {
namespace communication {

class Shared_Address_space {
public:
		virtual ~Shared_Address_space() {};
		virtual void put(void* data, int dst_id, int count, int offset, MPI_Datatype datatype) = 0;
		virtual void put(boost::mpi::packed_oarchive& ar, int dst_id, int offset) = 0;
		virtual void get(void* data, int src_id, int count, int offset, MPI_Datatype datatype) = 0;
		virtual void get(boost::mpi::packed_iarchive& ar, int src_id, int offset) = 0;
};
/*
class Shared_Variable {
public:
		virtual ~Shared_Variable() {};
		virtual void put(void* data, int dst_id);
		virtual void put(boost::mpi::packed_oarchive& ar, int dst_id);
		virtual void get(void* data, int src_id);
		virtual void get(boost::mpi::packed_iarchive& ar);
};
*/
class CommInterface {
public:
    virtual ~CommInterface() {};
    //User should also implement a create function for the CommInterface factory
		virtual mutex* new_lock() = 0;
		virtual Shared_Address_space* new_shared_space(unsigned int size) = 0;
    virtual void send(int dst_id, int tag, int count, MPI_Datatype datatype, void* data) = 0;
    virtual void send(int dst_id, int tag, boost::mpi::packed_oarchive& ar) = 0;
    virtual void recv(int src_id, int tag, int count, MPI_Datatype datatype, void* data) = 0;
    virtual void recv(int src_id, int tag, boost::mpi::packed_iarchive& ar) = 0;
    virtual int get_procId() = 0;
    virtual int size() = 0;
};

}//end of namespace communication
}//end of namespace futures

#endif

