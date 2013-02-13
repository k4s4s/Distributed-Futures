
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <string>
#include <map>
#include <vector>
#include "communication/communication.hpp"
#include "communication/commManager.hpp"
#include "mem/SharedMemoryManager.hpp"
#include "scheduler/scheduler.hpp"
#include "scheduler/schedManager.hpp"
#include "future_fwd.hpp"
#include "details.hpp"
#include "mpi.h"
#include "MPIMutex.hpp"
#include "common.hpp"

namespace futures {

void Futures_Initialize(int &argc, char**& argv);
int Futures_Id();
void Futures_Finalize();

class Futures_Environment { //singleton class
private:
    static Futures_Environment* pinstance;
    communication::CommManager* commManager;
    scheduler::SchedManager* schedManager;
    communication::CommInterface* commInterface;
    scheduler::Scheduler* sched;
		mem::Shared_Memory_manager* memManager;
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
		void Finalize();
    static Futures_Environment* Instance();
    int get_procId();
    int get_avaibleWorker();
		mem::Shared_pointer alloc(int id, int size);
		void free(int id,	mem::Shared_pointer ptr);
    template<typename T>
    void send_data(int dst_id, T data);
    template<typename T>
    T recv_data(int src_id);
		template<typename T>
		void set_data(T data, int dst_id, mem::Shared_pointer ptr, int size, int offset);
		template<typename T>
		T get_data(int dst_id, mem::Shared_pointer ptr, int size, int offset);	
		bool schedule_job(int dst_id, _stub *job);
    void wait_for_job();
		void execute_pending_jobs();
};

template<typename T>
void Futures_Environment::send_data(int dst_id, T data) {
    details::_send_data<T>()(commInterface, dst_id, 0,
                             data, details::is_primitive_type<T>());
};

template<typename T>
T Futures_Environment::recv_data(int src_id) {
    return details::_recv_data<T>()(commInterface, src_id, 0,
                                    details::is_primitive_type<T>());
};

template<typename T>
void Futures_Environment::set_data(T data, int dst_id, mem::Shared_pointer ptr, int size, int offset) {
	memManager->memset(data, dst_id, ptr, size, offset);
};

template<typename T>
T Futures_Environment::get_data(int dst_id, mem::Shared_pointer ptr, int size, int offset) {
	return memManager->memget<T>(dst_id, ptr, size, offset);
};

}//end of futures namespace

#endif

