
#include <mpi.h>

#include "futures_enviroment.hpp"
#include "future.hpp"

#include <iostream>

#define MASTER 0

using namespace std;
using namespace futures;

int helloWorld() {
	int id = Futures_Enviroment::Instance()->get_procId();
	cout << "Worker " << id << ":Hello Master" << endl;
	return id;
}

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "ARMCI");
	int id = env->get_procId();
	
	Future<int> *message = async<int>(1, 0, 1, sizeof(int), helloWorld);
	cout << "Moving along #" << id << endl;

	if(id == MASTER) {
		cout << "Master waits for answer" << endl;
		cout << "Master:Hello " << message->get() << endl;
	}
	else {
		cout << "Worker computes answer" << endl;
	}
	
	delete message;
	delete env;
}

