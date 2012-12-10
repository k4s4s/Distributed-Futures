
#include "futures.hpp"
#include <iostream>


#define MASTER 0

using namespace std;
using namespace futures;

class helloWorld {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	helloWorld() {};
	~helloWorld() {};
	int operator()() { 
		int id = Futures_Enviroment::Instance()->get_procId();
		cout << "- Worker" << id << ":Hello Master" << endl;
		return id;
	};
};

int helloWorld_func() { 
	int id = Futures_Enviroment::Instance()->get_procId();
	cout << "- Worker" << id << ":Hello Master" << endl;
	return id;
};

FUTURES_EXPORT_FUNCTOR(helloWorld);

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	helloWorld f;
	Future<int> *message = async(f);

	cout << "- Master :Hello " << message->get() << endl;

	delete message;
	env->Finalize();
};

