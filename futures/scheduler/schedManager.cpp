
#include "schedManager.hpp"
#include <iostream>

using namespace futures;
using namespace futures::communication;
using namespace futures::scheduler;

SchedManager* SchedManager::pinstance;

SchedManager::SchedManager() {};

SchedManager::~SchedManager() {
	schedulerMap.clear();
};

SchedManager* SchedManager::Instance() {
	if(pinstance == NULL) {
		pinstance = new SchedManager();
	}
	return pinstance;
};

void SchedManager::registerScheduler(const std::string &schedulerName, 
																				schedulerFn pfnCreate) {
	schedulerMap[schedulerName] = pfnCreate;
};
   	
Scheduler* SchedManager::createScheduler(const std::string &schedulerName, 
																						CommInterface *commInterface) { 
	std::map<std::string, schedulerFn>::iterator fnIt;
	fnIt = schedulerMap.find(schedulerName);
	if (fnIt != schedulerMap.end())
		return fnIt->second(commInterface);
	else
		return NULL; //TODO: throw an exception
};

