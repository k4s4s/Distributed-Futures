
#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <mutex.hpp>

#ifdef V_MPI
#include <MPIComm.hpp>
#endif

/*
namespace futures {
namespace communication {

class Shared_Address_space {
public:
		Shared_Address_space();
		virtual ~Shared_Address_space();
		template<typename T>
		void put(T& data, int dst_id, int count, int offset) {};
		template<typename T>
		T& get(void* data, int src_id, int count, int offset) {};
};

class CommInterface {
public:
    virtual ~CommInterface() {};
    //User should also implement a create function for the CommInterface factory
		virtual mutex* new_lock() = 0;
		virtual Shared_Address_space* new_shared_space(std::size_t size) = 0;
    virtual void send(int dst_id, int tag, int count, MPI_Datatype datatype, void* data) = 0;
    virtual void send(int dst_id, int tag, boost::mpi::packed_oarchive& ar) = 0;
    virtual void recv(int src_id, int tag, int count, MPI_Datatype datatype, void* data) = 0;
    virtual void recv(int src_id, int tag, boost::mpi::packed_iarchive& ar) = 0;
    virtual int get_procId() = 0;
    virtual int size() = 0;
};

}//end of namespace communication
}//end of namespace futures
*/
#endif

