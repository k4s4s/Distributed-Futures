
#include "futures_environment.hpp"
#include "scheduler/RR.hpp"
#include "communication/MPIComm.hpp"
#include "details.hpp"
#include <boost/mpi.hpp>
#include <iostream>
#include "common.hpp"

#define NEW_TASK 2001
#define EXIT -1

#define STATUS_OFFSET 0
#define DATA_OFFSET sizeof(int)

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
				else  {
					START_TIMER("user_code_execution_time");
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
    //Initilize communication Interface
    commInterface = new communication::CommInterface(argc, argv);
		START_TIMER("total_execution_time");
		START_TIMER("initialization_time");
		//Create shared address space
		memManager = new mem::Shared_Memory_manager(commInterface);
    schedManager = scheduler::SchedManager::Instance();
    schedManager->registerScheduler("RR", scheduler::RRScheduler::create);
    sched = schedManager->createScheduler(schedulerName, commInterface);
		STOP_TIMER("initialization_time");
};

Futures_Environment::~Futures_Environment() {};

void Futures_Environment::Finalize() {
		STOP_TIMER("user_code_execution_time");
		START_TIMER("finalization_time");
    if(commInterface->get_procId() == 0) {
        /*maybe not necessary,
					terminate would also return
					correct value for workers
					in order to terminate*/
        while(!sched->terminate());
        DPRINT_MESSAGE("ENVIROMENT: master exiting program");
    }
		int id = commInterface->get_procId();
    delete sched;
		delete memManager;
		STOP_TIMER("finalization_time");
		STOP_TIMER("total_execution_time");
		PRINT_STATS(commInterface);	
    delete commInterface;
    //delete pinstance;
};

int Futures_Environment::get_procId() {
    return commInterface->get_procId();
};

int Futures_Environment::get_avaibleWorker() {
    int worker_id = sched->nextAvaibleWorkerId();
    return worker_id;
};

mem::Shared_pointer Futures_Environment::alloc(int id, int size) {
	mem::Shared_pointer ptr = memManager->allocate(id, size);
	return ptr;
};

void Futures_Environment::free(int id, mem::Shared_pointer ptr) {
	memManager->free(id, ptr);
};

bool Futures_Environment::schedule_job(int dst_id, _stub *job) {
	return sched->schedule_job(dst_id, job);
};

void Futures_Environment::wait_for_job() {
		START_TIMER("idle_time");
		while(!sched->terminate()) {
			sched->run_proc();
		}
		STOP_TIMER("idle_time");
    DPRINT_MESSAGE("ENVIRONMENT: worker exiting program");
		this->Finalize();
};

void Futures_Environment::execute_pending_jobs() {
	sched->run_proc();
}

