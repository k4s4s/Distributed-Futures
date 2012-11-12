
#ifndef FUTURES_ENVIROMENT_H
#define FUTURES_ENVIROMENT_H

#include <string>
#include <map>
#include "communication/communication.hpp"
#include "communication/commManager.hpp"
#include "communication/mpi_comm.hpp"
#include "communication/armci_comm.hpp"

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
    /* hold information about different futures. Attention: it is possible that
    enviroment are different accross processes */
    static std::map<unsigned int, communication::SharedDataManager*> futuresMap;
    static unsigned int total_futures; //never decrease that value, it is  not actual number of futures, just next avaible id for the map
    static Futures_Enviroment* pinstance;
protected:
    Futures_Enviroment(int &argc, char**& argv, const std::string& commInterfaceName);
		communication::CommManager* commManager;
    communication::CommInterface* commInterface;
public:
    ~Futures_Enviroment();
    static Futures_Enviroment* Initialize(int &argc, char**& argv, const std::string& commInterfaceName);
    static Futures_Enviroment* Instance();
    MPI_Comm get_communicator();
    communication::SharedDataManager* get_SharedDataManager(unsigned int id);
    // this function returns the id used to register the future in the map
    unsigned int registerFuture(unsigned int _data_size, unsigned int _type_size);
    void removeFuture(unsigned int id);
    int get_procId();
};

/*** Future_Enviroment impelementation ***/
std::map<unsigned int, communication::SharedDataManager*> Futures_Enviroment::futuresMap;
unsigned int Futures_Enviroment::total_futures = 0;
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
    total_futures = 0;
};

Futures_Enviroment::~Futures_Enviroment() {
		//TODO: free map, future should have cleared out his shared data though
    delete commInterface;
		delete commManager;
    pinstance = NULL;
};

communication::SharedDataManager* Futures_Enviroment::get_SharedDataManager(unsigned int id) {
    return futuresMap[id];
};

unsigned int Futures_Enviroment::registerFuture(unsigned int _data_size, unsigned int _type_size) {
    unsigned int id = total_futures;
    total_futures++;
    futuresMap[id] = commInterface->new_sharedDataManager(_data_size, _type_size);
    return id;
};

void Futures_Enviroment::removeFuture(unsigned int id) {
    communication::SharedDataManager *sharedDataManager = futuresMap[id];
    futuresMap.erase(id);
    delete sharedDataManager;
};

int Futures_Enviroment::get_procId() {
    return commInterface->get_procId();
};

}//end of futures namespace

#endif

