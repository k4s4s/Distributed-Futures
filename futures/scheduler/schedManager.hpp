
#ifndef _SCHEDMANAGER_H
#define _SCHEDMANAGER_H

#include <string>
#include <map>
#include "scheduler.hpp"
#include "../communication/communication.hpp"

namespace futures {
namespace scheduler {

typedef Scheduler* (*schedulerFn)(communication::CommInterface*);

class SchedManager {
private:
    SchedManager();
    std::map<std::string, schedulerFn> schedulerMap;
		static SchedManager *pinstance;
public:
    ~SchedManager();
    static SchedManager* Instance();
    void registerScheduler(const std::string &schedulerName, schedulerFn pfnCreate);
   	Scheduler *createScheduler(const std::string &schedulerName, 
															communication::CommInterface* commInterface);
};

}//end of communication namespace
}//end of futures namespace

#endif

