
#include "futures_enviroment.hpp"
#include "future.hpp"
#include "promise.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/mpi.hpp>
#include <iostream>

#include <mpi.h>

#define SIZE 10

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
	answer_to_the_question(int _value): value(_value) {};
	~answer_to_the_question() {};
	int get_value() { return value; };
};

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
	
	answer_to_the_question answer[SIZE];
	for(int i=0; i < SIZE; i++) {
		answer[i] = answer_to_the_question(i);
	}

	//Future<int> *message = async<>(helloWorld);
	Promise<answer_to_the_question*> *prom = new Promise<answer_to_the_question*>(1, 0, SIZE);
	Future<answer_to_the_question*> *message = prom->get_future();
	if(id == MASTER) {
		answer_to_the_question *ans;
		ans = message->get();
		for(int i=0; i < SIZE; i++) {
			if(answer[i].get_value() != ans[i].get_value()) {
				cout << "Test Failed" << endl;
				break;
			}
			else if(i == SIZE-1) cout << "Test Passed" << endl;
			//cout << "- The answer to the question is " << ans[i].get_value() << endl;
		}
		//delete ans;	FIXME: delete here causes a memory error in glibc
	}
	else if(id == 1) {
	  prom->set_value(answer);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	delete prom;
	delete message;
	delete env;
}

