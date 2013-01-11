
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
    //commManager->registerCommInterface("ARMCI", communication::ARMCIComm::create);
    //commManager->registerCommInterface("MPIAsync", communication::MPIAsyncComm::create);
    //Initilize communication Interface
    commInterface = commManager->createCommInterface(commInterfaceName, argc, argv);
    schedManager = scheduler::SchedManager::Instance();
    schedManager->registerScheduler("RR", scheduler::RRScheduler::create);
    sched = schedManager->createScheduler(schedulerName, commInterface);
		statManager->stop_timer("initialization_time");
};

Futures_Environment::Futures_Environment(int &argc, char**& argv) {
    //Initilize communication manager and register default interfaces
    commManager = communication::CommManager::Instance();
    commManager->registerCommInterface("MPI", communication::MPIComm::create);
    commInterface = commManager->createCommInterface("MPI", argc, argv);
    schedManager = scheduler::SchedManager::Instance();
    schedManager->registerScheduler("RR", scheduler::RRScheduler::create);
    sched = schedManager->createScheduler("RR", commInterface);
};

Futures_Environment::~Futures_Environment() {};

void Futures_Environment::Finalize() {
		statManager->start_timer("finalization_time");
    if(commInterface->get_procId() == 0) {
        /*maybe not necessary,
        															terminate would also return
        															correct value for workers
        															in order to terminate*/
        do {
            sched->set_status(scheduler::ProcStatus::TERMINATED);
            for(int i=1; i < commInterface->size(); i++) {
                this->send_data(i, EXIT);
						}
        } while(!sched->terminate());
        DPRINT_MESSAGE("ENVIROMENT: master exiting program");
    }
		statManager->stop_timer("finalization_time");
		statManager->stop_timer("total_execution_time");
		statManager->print_stats();	
    delete sched;
    delete commInterface;
    delete commManager;	
		delete statManager;
    //delete pinstance;
};

communication::SharedDataManager*
Futures_Environment::new_SharedDataManager(int _src_id, int _dst_id,
        int _data_size, int _type_size,
        MPI_Datatype _datatype) {
    return commInterface->new_sharedDataManager(_src_id, _dst_id,
            _data_size, _type_size, _datatype);
};

int Futures_Environment::get_procId() {
    return commInterface->get_procId();
};

int Futures_Environment::get_avaibleWorker() {
    int worker_id = sched->nextAvaibleWorkerId();
    return worker_id;
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

void Futures_Environment::wait_for_job() {
    while(sched->terminate()) {
        DPRINT_MESSAGE("ENVIROMENT: worker waiting for job");
				statManager->start_timer("idle_time");
        int master_id = this->recv_data<int>(MPI_ANY_SOURCE);
				statManager->stop_timer("idle_time");
        if(master_id == EXIT) break;
        _stub *job = this->recv_job(master_id);
        job->run(master_id);
        sched->set_status(scheduler::ProcStatus::IDLE);
    }
    DPRINT_MESSAGE("ENVIROMENT: worker exiting program");
    sched->set_status(scheduler::ProcStatus::TERMINATED);
		this->Finalize();
};

