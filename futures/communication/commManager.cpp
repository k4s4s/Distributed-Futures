
#include "commManager.hpp"
#include <iostream>

using namespace futures;
using namespace futures::communication;

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

