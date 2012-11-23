
#include "futures_enviroment.hpp"
#include "scheduler/RR.hpp"
#include "communication/MPIComm.hpp"
//#include "communication/ARMCIComm.hpp"
//#include "communication/MPIAsyncComm.hpp"

using namespace futures;
/*** Future_Enviroment impelementation ***/
Futures_Enviroment* Futures_Enviroment::pinstance = NULL;// initialize pointer

Futures_Enviroment* Futures_Enviroment::Initialize(int &argc, char**& argv,
																									const std::string& commInterfaceName,
																									const std::string& schedulerName) {
    if (!pinstance) {
        pinstance = new Futures_Enviroment(argc, argv, commInterfaceName, schedulerName);
		}
    return pinstance; // address of sole instance
};

Futures_Enviroment* Futures_Enviroment::Initialize(int &argc, char**& argv) {
    if (!pinstance) {
        pinstance = new Futures_Enviroment(argc, argv); // create sole instance
    }
    return pinstance; // address of sole instance
};

Futures_Enviroment* Futures_Enviroment::Instance () {

    return pinstance; // address of sole instance
};

Futures_Enviroment::Futures_Enviroment(int &argc, char**& argv,
                                      const std::string& commInterfaceName,
																			const std::string& schedulerName) {
		//Initilize communication manager and register default interfaces
		commManager = communication::CommManager::Instance();
		commManager->registerCommInterface("MPI", communication::MPIComm::create);
		//commManager->registerCommInterface("ARMCI", communication::ARMCIComm::create);
		//commManager->registerCommInterface("MPIAsync", communication::MPIAsyncComm::create);
		//Initilize communication Interface
    commInterface = commManager->createCommInterface(commInterfaceName, argc, argv);
		schedManager = scheduler::SchedManager::Instance();
		schedManager->registerScheduler("RR", scheduler::RRScheduler::create);	
		sched = schedManager->createScheduler(schedulerName, commInterface);
};

Futures_Enviroment::Futures_Enviroment(int &argc, char**& argv) {
		//Initilize communication manager and register default interfaces
		commManager = communication::CommManager::Instance();
		commManager->registerCommInterface("MPI", communication::MPIComm::create);
    commInterface = commManager->createCommInterface("MPI", argc, argv);
		schedManager = scheduler::SchedManager::Instance();
		schedManager->registerScheduler("RR", scheduler::RRScheduler::create);	
		sched = schedManager->createScheduler("RR", commInterface);
};

Futures_Enviroment::~Futures_Enviroment() {
    delete commInterface;
		delete commManager;
    pinstance = NULL;
};

communication::SharedDataManager* 
Futures_Enviroment::new_SharedDataManager(int _src_id, int _dst_id, 
																					int _data_size, int _type_size,
																					MPI_Datatype _datatype) {
		return commInterface->new_sharedDataManager(_src_id, _dst_id,
																								_data_size, _type_size, _datatype);
};

int Futures_Enviroment::get_procId() {
    return commInterface->get_procId();
};

int Futures_Enviroment::get_avaibleWorker() {
		return sched->nextAvaibleWorkerId();
};

