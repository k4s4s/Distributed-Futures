
#include "futures_environment.hpp"
#include "scheduler/RR.hpp"
#include "communication/MPIComm.hpp"
//#include "communication/ARMCIComm.hpp"
//#include "communication/MPIAsyncComm.hpp"
#include "details.hpp"
#include "boost/mpi.hpp"
#include <iostream>
#include "common.hpp"

#define NEW_TASK 2001
#define EXIT -1

using namespace futures;

/* environment wrappers */
Futures_Environment *_global_environment;

void futures::Futures_Initialize(int &argc, char**& argv) {
	Futures_Environment::Initialize(argc, argv, "MPI", "RR");
};

int futures::Futures_Id() {
	return Futures_Environment::Instance()->get_procId();
};

void futures::Futures_Finalize() {
	Futures_Environment::Instance()->Finalize();
};

/*** Future_Environment impelementation ***/
Futures_Environment* Futures_Environment::pinstance = NULL;// initialize pointer

Futures_Environment* Futures_Environment::Initialize(int &argc, char**& argv,
        const std::string& commInterfaceName,
        const std::string& schedulerName) {
    if (!pinstance) {
        DPRINT_MESSAGE("ENVIROMENT: initializing");
        pinstance = new Futures_Environment(argc, argv, commInterfaceName, schedulerName);
        int id = pinstance->get_procId();
        if(id != 0) {
            pinstance->wait_for_job();
            delete pinstance;
            exit(1);
        }
    }
    return pinstance; // address of sole instance
};

Futures_Environment* Futures_Environment::Initialize(int &argc, char**& argv) {
    if (!pinstance) {
        pinstance = new Futures_Environment(argc, argv); // create sole instance
        int id = pinstance->get_procId();
        if(id != 0) {
            pinstance->wait_for_job();
            delete pinstance;
            exit(1);
        }
    }
    return pinstance; // address of sole instance
};

Futures_Environment* Futures_Environment::Instance () {
    return pinstance; // address of sole instance
};

Futures_Environment::Futures_Environment(int &argc, char**& argv,
                                       const std::string& commInterfaceName,
                                       const std::string& schedulerName) {
		statManager = stats::StatManager::Instance();
		statManager->start_timer("total_execution_time");
		statManager->start_timer("initialization_time");
    //Initilize communication manager and register default interfaces
    commManager = communication::CommManager::Instance();
    commManager->registerCommInterface("MPI", communication::MPIComm::create);
    //Initilize communication Interface
    commInterface = commManager->createCommInterface(commInterfaceName, argc, argv);
		//Create shared address space
		sharedMemory = new communication::MPI_Shared_memory();
    schedManager = scheduler::SchedManager::Instance();
    schedManager->registerScheduler("RR", scheduler::RRScheduler::create);
    sched = schedManager->createScheduler(schedulerName, commInterface);
		statManager->stop_timer("initialization_time");
};

/*TODO: delete this... */
Futures_Environment::Futures_Environment(int &argc, char**& argv) {
    //Initilize communication manager and register default interfaces
    commManager = communication::CommManager::Instance();
    commManager->registerCommInterface("MPI", communication::MPIComm::create);
    commInterface = commManager->createCommInterface("MPI", argc, argv);
    schedManager = scheduler::SchedManager::Instance();
    schedManager->registerScheduler("RR", scheduler::RRScheduler::create);
};

Futures_Environment::~Futures_Environment() {};

void Futures_Environment::Finalize() {
		statManager->start_timer("finalization_time");
    if(commInterface->get_procId() == 0) {
        /*maybe not necessary,
        															terminate would also return
        															correct value for workers
        															in order to terminate*/
        while(!sched->terminate());
        DPRINT_MESSAGE("ENVIROMENT: master exiting program");
    }
		statManager->stop_timer("finalization_time");
		statManager->stop_timer("total_execution_time");
		statManager->print_stats();	
    delete sched;
		delete sharedMemory;
    delete commInterface;
    delete commManager;	
		delete statManager;
    //delete pinstance;
};

communication::Shared_data*
Futures_Environment::new_Shared_data(int _dst_id,
																		unsigned int _base,
                    								unsigned int _data_size, unsigned int _type_size,
                    								MPI_Datatype _datatype, MPI_Win _data_win, 
																		MPIMutex* _data_lock) {
    return commInterface->new_Shared_data(_dst_id, _base, _data_size, _type_size, 
																					_datatype, _data_win, _data_lock);
};

int Futures_Environment::get_procId() {
    return commInterface->get_procId();
};

int Futures_Environment::get_avaibleWorker() {
    int worker_id = sched->nextAvaibleWorkerId();
    return worker_id;
};

MPI_Win Futures_Environment::get_data_window() {
	return sharedMemory->get_data_window();
};

MPIMutex *Futures_Environment::get_data_lock() {
	return sharedMemory->get_data_lock();
};

int Futures_Environment::alloc(int _size) {
	int base_address = sharedMemory->allocate(_size);
	return base_address;
};

void Futures_Environment::free(communication::Shared_data *sharedData) {
	delete sharedData;
};

void Futures_Environment::send_job(int dst_id, _stub *job) {
    boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
    _stub_wrapper tw(job);
    oa << tw;
    commInterface->send(dst_id, NEW_TASK, oa);
		
};

_stub *Futures_Environment::recv_job(int src_id) {
    sched->set_status(scheduler::ProcStatus::RUNNING);
    boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
    commInterface->recv(src_id, NEW_TASK, ia);
    _stub_wrapper tw;
    ia >> tw;
    return tw.get_task();
};

bool Futures_Environment::schedule_job(int dst_id, _stub *job) {
	return sched->schedule_job(dst_id, job);
};

void Futures_Environment::wait_for_job() {
		sched->schedule_proc();
    DPRINT_MESSAGE("ENVIROMENT: worker exiting program");
		this->Finalize();
};

