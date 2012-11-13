
#ifndef _COMMMANAGER_H
#define _COMMMANAGER_H

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

}//end of communication namespace
}//end of futures namespace

#endif

