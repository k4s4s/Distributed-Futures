
#include "logger.hpp"

using namespace std;
using namespace futures;

logger *logger::pinstance = NULL;

logger::logger() {
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	stringstream ss;
  ss << id;
	outFile = new ofstream(("log"+ss.str()+".txt"));
};

logger::~logger() {
	delete outFile;
};

logger *logger::Instance() {
	if(pinstance == NULL)
		pinstance = new logger();
	return pinstance;
};

ofstream *logger::out() {
	return outFile;
};
