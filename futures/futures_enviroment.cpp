
#include "futures_enviroment.hpp"
#include "communication/MPIComm.hpp"
#include "communication/ARMCIComm.hpp"
#include "communication/MPIAsyncComm.hpp"

using namespace futures;
/*** Future_Enviroment impelementation ***/
Futures_Enviroment* Futures_Enviroment::pinstance = NULL;// initialize pointer

Futures_Enviroment* Futures_Enviroment::Initialize(int &argc, char**& argv,
																									const std::string& commInterfaceName) {
    if (!pinstance) {
        pinstance = new Futures_Enviroment(argc, argv, commInterfaceName); // create sole instance
    }
    return pinstance; // address of sole instance
};

Futures_Enviroment* Futures_Enviroment::Instance () {

    return pinstance; // address of sole instance
};

Futures_Enviroment::Futures_Enviroment(int &argc, char**& argv,
                                       const std::string& commInterfaceName) {
		//Initilize communication manager and register default interfaces
		commManager = communication::CommManager::Instance();
		commManager->registerCommInterface("MPI", communication::MPIComm::create);
		commManager->registerCommInterface("ARMCI", communication::ARMCIComm::create);
		commManager->registerCommInterface("MPIAsync", communication::MPIAsyncComm::create);
		//Initilize communication Interface
    commInterface = commManager->createCommInterface(commInterfaceName, argc, argv);
};

Futures_Enviroment::~Futures_Enviroment() {
		//TODO: free map, future should have cleared out his shared data though
    delete commInterface;
		delete commManager;
    pinstance = NULL;
};

<<<<<<< HEAD
communication::SharedDataManager* 
Futures_Enviroment::new_SharedDataManager(int _src_id, int _dst_id, 
																					int _data_size, int _type_size) {
		return commInterface->new_sharedDataManager(_src_id, _dst_id,
																								_data_size, _type_size);
=======
communication::SharedDataManager* Futures_Enviroment::get_SharedDataManager(unsigned int id) {
    return futuresMap[id];
};

unsigned int Futures_Enviroment::registerFuture(unsigned int _data_size, unsigned int _type_size) {
    unsigned int id = total_futures;
    total_futures++;
    futuresMap[id] = commInterface->new_sharedDataManager(_data_size, _type_size, id);
    return id;
};

void Futures_Enviroment::removeFuture(unsigned int id) {
    communication::SharedDataManager *sharedDataManager = futuresMap[id];
    futuresMap.erase(id);
    delete sharedDataManager;
>>>>>>> mpi-async-comm
};

int Futures_Enviroment::get_procId() {
    return commInterface->get_procId();
};

