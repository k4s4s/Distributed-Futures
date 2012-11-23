
#include "futures_enviroment.hpp"
#include "future.hpp"
#include "promise.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/mpi.hpp>
#include <iostream>

#include <mpi.h>

class answer_to_the_question {
private:
	int value;
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & value;
	};
public:
	answer_to_the_question(): value(42) {};
	~answer_to_the_question() {};
	int get_value() { return value; };
};

#define MASTER 0

using namespace std;
using namespace futures;

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	
	//Future<int> *message = async<>(helloWorld);
	Promise<answer_to_the_question> *prom = new Promise<answer_to_the_question>(1, 0);
	Future<answer_to_the_question> *message = prom->get_future();
	if(id == MASTER) {
		answer_to_the_question answer = message->get();
		cout << "- The answer to the question is " << answer.get_value() << endl;
	}
	else if(id == 1) {
		answer_to_the_question answer = answer_to_the_question();
	  prom->set_value(answer);
	}

	delete prom;
	delete message;
	delete env;
}

