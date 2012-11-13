
#include "futures_enviroment.hpp"
#include "future.hpp"

#include <iostream>

#define MASTER 0

using namespace std;
using namespace futures;

int helloWorld() {
	int id = Futures_Enviroment::Instance()->get_procId();
	cout << "- Worker" << id << ":Hello Master" << endl;
	return id;
}

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI");
	int id = env->get_procId();
	
	Future<int> *message = async<int>(1, 0, 1, sizeof(int), helloWorld);

	if(id == MASTER) {
		cout << "- Master :Hello " << message->get() << endl;
	}
	
	delete message;
	delete env;
}

