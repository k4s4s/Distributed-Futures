
#include "futures_enviroment.hpp"
#include "communication/MPIComm.hpp"
#include "communication/ARMCIComm.hpp"

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
		//Initilize communication Interface
    commInterface = commManager->createCommInterface(commInterfaceName, argc, argv);
};

Futures_Enviroment::~Futures_Enviroment() {
		//TODO: free map, future should have cleared out his shared data though
    delete commInterface;
		delete commManager;
    pinstance = NULL;
};

communication::SharedDataManager* 
Futures_Enviroment::new_SharedDataManager(int _src_id, int _dst_id, 
																					int _data_size, int _type_size) {
		return commInterface->new_sharedDataManager(_src_id, _dst_id,
																								_data_size, _type_size);
};

int Futures_Enviroment::get_procId() {
    return commInterface->get_procId();
};

