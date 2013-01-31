
#ifndef _LOGGER_H
#define _LOGGER_H

#include <fstream>
#include <mpi.h>
#include <sstream>

namespace futures {

class logger {
private:
	static logger *pinstance;
	std::ofstream *outFile;
	logger();
public:
	~logger();
	static logger *Instance();
	std::ofstream *out();
};

}

#endif 
