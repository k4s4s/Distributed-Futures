
#ifndef _MPICOMM_H
#define _MPICOMM_H

#include <mpi.h>
#include "communication.hpp"

namespace futures {
namespace communication {

class MPI_Shared_address_space : public Shared_Address_space {
private:
	MPI_Win shared_memory_win;
	void *shared_memory;
public:
	MPI_Shared_address_space(unsigned int size);
	~MPI_Shared_address_space();
  void put(void* data, int dst_id, int count, int offset, MPI_Datatype datatype);
  void put(boost::mpi::packed_oarchive& ar, int dst_id, int offset);
  void get(void* data, int src_id, int count, int offset, MPI_Datatype datatype);
  void get(boost::mpi::packed_iarchive& ar, int src_id, int offset);
};

class MPIComm : public CommInterface {
public:
    MPIComm(int &argc, char**& argv);
    ~MPIComm();
    static CommInterface* create(int &argc, char**& argv);
		mutex* new_lock();
		Shared_Address_space* new_shared_space(unsigned int size);
    void send(int dst_id, int tag, int count, MPI_Datatype datatype, void* data);
    void send(int dst_id, int tag, boost::mpi::packed_oarchive& ar);
    void recv(int src_id, int tag, int count, MPI_Datatype datatype, void* data);
    void recv(int src_id, int tag, boost::mpi::packed_iarchive& ar);
    int get_procId();
    int size();
};

}//end of namespace communication
}//end of namespace futures

#endif

