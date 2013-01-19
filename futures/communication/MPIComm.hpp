
#ifndef _MPICOMM_H
#define _MPICOMM_H

#include <mpi.h>
#include "communication.hpp"
#include "MPIMutex.hpp"
#include "mpi_details.hpp"

namespace futures {
namespace communication {

class MPI_Shared_data : public Shared_data {
private:
    MPI_Comm comm;
		MPI_Win data_win;
		MPIMutex *data_lock;
    int src_id;
    int dst_id;
		unsigned int base;
		unsigned int limit;
    unsigned int data_size;
    unsigned int type_size;
    MPI_Datatype datatype;
		int status;
		int ar_size;
public:
    MPI_Shared_data(int _dst_id,
										unsigned int _base,
                    unsigned int _data_size, unsigned int _type_size,
                    MPI_Datatype _datatype, MPI_Win _data_win, MPIMutex* _data_lock);
    ~MPI_Shared_data();
    unsigned int get_dataSize();
    void get_data(void* val);
    void get_data(boost::mpi::packed_iarchive& ar);
    void set_data(void* val);
    void set_data(boost::mpi::packed_oarchive& ar);
    void get_status(int *val);
    void set_status(int *val);
    MPI_Comm get_comm();
};

class MPIComm : public CommInterface {
public:
    MPIComm(int &argc, char**& argv);
    ~MPIComm();
    static CommInterface* create(int &argc, char**& argv);
    Shared_data* new_Shared_data(int _dst_id,
																unsigned int _base,
            										unsigned int _data_size, unsigned int _type_size,
            										MPI_Datatype _datatype, MPI_Win _data_win, 
																MPIMutex* _data_lock);
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

