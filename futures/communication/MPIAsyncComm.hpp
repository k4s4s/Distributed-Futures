
#ifndef _MPIAsyncComm_H
#define _MPIAsyncComm_H

#include <mpi.h>
#include "communication.hpp"
#include "MPIMutex.hpp"

namespace futures {
namespace communication {

namespace details {
//TODO:implement actual mutexes
static void wait_and_recv(void *buf, int count, MPI_Datatype datatype,
    										int source, int tag, MPI_Comm comm, 
												MPI_Status *status, MPI_Request *request);

static void wait_and_send(void *buf, int count, MPI_Datatype datatype, 
													int dest,	int tag, MPI_Comm comm, 
													MPI_Request *request, MPI_Status *status);

}//end of details namespace

class MPIAsyncSharedDataManager : public SharedDataManager {
private:
    MPI_Status data_status;
		MPI_Request data_request;
    MPI_Status 	readyFlag_status;
		MPI_Request readyFlag_request;
    int readyFlag;
    unsigned int data_size;
    unsigned int type_size;
		int id;
public:
    MPIAsyncSharedDataManager(unsigned int _data_size, unsigned int _type_size, int _id);
    ~MPIAsyncSharedDataManager();
		unsigned int get_dataSize();
    void get_data(void* val, int rank);
    void set_data(void* val, int rank);
    void get_status(int *val, int rank);
    void set_status(int *val, int rank);
};

class MPIAsyncComm : public CommInterface {
public:
    MPIAsyncComm(int &argc, char**& argv);
    ~MPIAsyncComm();
		static CommInterface* create(int &argc, char**& argv);
		SharedDataManager* new_sharedDataManager(unsigned int _data_size, unsigned int _type_size, int _id);
    int get_procId();
};

}//end of namespace communication
}//end of namespace futures

#endif

