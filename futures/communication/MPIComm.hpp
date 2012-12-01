
#ifndef _MPICOMM_H
#define _MPICOMM_H

#include <mpi.h>
#include "communication.hpp"
#include "MPIMutex.hpp"
#include "mpi_details.hpp"

namespace futures {
namespace communication {

class MPISharedDataManager : public SharedDataManager {
private:
    MPI_Win data_win;
		MPI_Win ar_size_win;
    MPI_Win status_win;
		MPIMutex* data_lock;
		MPIMutex* ar_size_lock;
		MPIMutex* status_lock;
    void *data; //would be nice to have this as an template instead of void*
    int status;
		int ar_size;
    unsigned int data_size; //maybe we do not need this one
    unsigned int type_size;
		MPI_Comm comm;
		int src_id;
		int dst_id;
		MPI_Datatype datatype;
public:
    MPISharedDataManager(int _src_id, int _dst_id, 
												unsigned int _data_size, unsigned int _type_size,
												MPI_Datatype datatype);
    ~MPISharedDataManager();
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
		SharedDataManager* new_sharedDataManager(int _src_id, int _dst_id, 
																						unsigned int _data_size, unsigned int _type_size,
																						MPI_Datatype _datatype);
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

