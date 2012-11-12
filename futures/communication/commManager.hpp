
#ifndef _COMMMANAGER_H
#define _COMMMANAGER_H

#include <iostream>

#include <string>
#include <map>
#include "communication.hpp"

namespace futures {
namespace communication {

typedef CommInterface* (*commInterfaceFn)(int &, char**&);

class CommManager {
private:
    CommManager();
    std::map<std::string, commInterfaceFn> commInterfaceMap;
		static CommManager *pinstance;
public:
    ~CommManager();
    static CommManager* Instance();
    void registerCommInterface(const std::string &commInterfaceName, 
															commInterfaceFn pfnCreate);
   	CommInterface *createCommInterface(const std::string &commInterfaceName, 
																			int &argc, char**& argv);
};

CommManager* CommManager::pinstance;

CommManager::CommManager() {};

CommManager::~CommManager() {
	commInterfaceMap.clear();
};

CommManager* CommManager::Instance() {
	if(pinstance == NULL) {
		pinstance = new CommManager();
	}
	return pinstance;
};

void CommManager::registerCommInterface(const std::string &commInterfaceName, 
																				commInterfaceFn pfnCreate) {
	commInterfaceMap[commInterfaceName] = pfnCreate;
};
   	
CommInterface* CommManager::createCommInterface(const std::string &commInterfaceName, int &argc, char**& argv) { 
	std::map<std::string, CommInterface* (*)(int &, char**&)>::iterator fnIt;
	fnIt = commInterfaceMap.find(commInterfaceName);
	if (fnIt != commInterfaceMap.end())
		return fnIt->second(argc, argv);
	else
		return NULL; //TODO: throw an exception
};

}//end of communication namespace
}//end of futures namespace

#endif

