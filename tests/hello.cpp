
#include "futures.hpp"
#include <iostream>


#define MASTER 0

using namespace std;
using namespace futures;

class helloWorld : public AsyncTask {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & boost::serialization::base_object<AsyncTask>(*this);
	};
public:
	helloWorld() {};
	~helloWorld() {};
	void operator()(int _id) { 
		TaskHandler taskHandler = init_async_task<int>(_id);
		int id = Futures_Enviroment::Instance()->get_procId();
		cout << "- Worker" << id << ":Hello Master" << endl;
		return_future(id, taskHandler);
		return;
	};
};

BOOST_CLASS_EXPORT(helloWorld);



int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	
	helloWorld f;
	Future<int> *message = async<int>(f);


	if(id == MASTER) {
		cout << "- Master :Hello " << message->get() << endl;
	}

	delete message;
	delete env;
}

