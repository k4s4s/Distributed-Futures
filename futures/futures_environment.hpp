
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <string>
#include <map>
#include <vector>
#include "communication/communication.hpp"
#include "communication/commManager.hpp"
#include "communication/MPISharedMemory.hpp"
#include "scheduler/scheduler.hpp"
#include "scheduler/schedManager.hpp"
#include "future_fwd.hpp"
#include "details.hpp"
#include "mpi.h"
#include "MPIMutex.hpp"
#include "common.hpp"

#define TASK_DATA 2003

namespace futures {

void Futures_Initialize(int &argc, char**& argv);
int Futures_Id();
void Futures_Finalize();

class Futures_Environment { //singleton class
private:
    static Futures_Environment* pinstance;
    communication::CommManager* commManager;
    communication::CommInterface* commInterface;
    scheduler::SchedManager* schedManager;
    scheduler::Scheduler* sched;
		stats::StatManager* statManager;
		communication::MPI_Shared_memory *sharedMemory;
protected:
    Futures_Environment(int &argc, char**& argv,
                       const std::string& commInterfaceName,
                       const std::string& schedulerName);
    Futures_Environment(int &argc, char**& argv);
public:
    ~Futures_Environment();
    static Futures_Environment* Initialize(int &argc, char**& argv,
                                          const std::string& commInterfaceName,
                                          const std::string& schedulerName);
    static Futures_Environment* Initialize(int &argc, char**& argv);
		void Finalize();
    static Futures_Environment* Instance();
    MPI_Comm get_communicator();
    communication::Shared_data* new_Shared_data(int _dst_id,
																								unsigned int _base,
																								unsigned int _data_size, unsigned int _type_size,
																								MPI_Datatype _datatype, MPI_Win _data_win, 
																								MPIMutex* _data_lock);
    int get_procId();
    int get_avaibleWorker();
		int alloc(int _size);
		void free(communication::Shared_data *sharedData);
		MPI_Win get_data_window();
		MPIMutex *get_data_lock();
    void send_job(int dst_id, _stub *job);
    _stub *recv_job(int src_id);
    template<typename T>
    void send_data(int dst_id, T data);
    template<typename T>
    T recv_data(int src_id);
		bool schedule_job(int dst_id, _stub *job);
    void wait_for_job();
		void execute_pending_jobs();
};

template<typename T>
void Futures_Environment::send_data(int dst_id, T data) {
    details::_send_data<T>()(commInterface, dst_id, TASK_DATA,
                             data, details::_is_mpi_datatype<T>());
};

template<typename T>
T Futures_Environment::recv_data(int src_id) {
    return details::_recv_data<T>()(commInterface, src_id, TASK_DATA,
                                    details::_is_mpi_datatype<T>());
};

}//end of futures namespace

#endif

