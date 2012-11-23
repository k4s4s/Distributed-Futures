
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <string>
#include <map>
#include "communication/communication.hpp"
#include "communication/commManager.hpp"
#include "scheduler/scheduler.hpp"

#ifdef DEBUG
#define DEBUG_MSG(str) std::cout << str << std::endl
#else
#define DEBUG_MSG(str) ;
#endif

namespace futures {

/*TODO: perhaps we could have the enviroment maintain another window, that will be used to contain futures, promises,
				 thus using RMA puts/gets to emulate shared memory model more closely. An issue here can be serialization */
class Futures_Enviroment { //singleton class
private:
    static Futures_Enviroment* pinstance;
		communication::CommManager* commManager;
    communication::CommInterface* commInterface;
		scheduler::Scheduler* sched;
protected:
    Futures_Enviroment(int &argc, char**& argv, const std::string& commInterfaceName);
public:
    ~Futures_Enviroment();
    static Futures_Enviroment* Initialize(int &argc, char**& argv, const std::string& commInterfaceName);
    static Futures_Enviroment* Instance();
    MPI_Comm get_communicator();
		communication::SharedDataManager* new_SharedDataManager(int _src_id, int _dst_id, 
																														int _data_size, int _type_size,
																														MPI_Datatype datatype);
    int get_procId();
		int get_avaibleWorker();
};

}//end of futures namespace

#endif

