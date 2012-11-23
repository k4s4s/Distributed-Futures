
#include "futures_enviroment.hpp"
#include "future.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/mpi.hpp>
#include <iostream>

#include <mpi.h>

#define MASTER 0

using namespace std;
using namespace futures;
namespace mpi = boost::mpi;

class async_task {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	virtual ~async_task() {};
	virtual int operator()() = 0;		
};


BOOST_SERIALIZATION_ASSUME_ABSTRACT(async_task);

class addOp : public async_task {
private:
  friend class boost::serialization::access;
	int x, y;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & boost::serialization::base_object<async_task>(*this);
		ar & x & y;
	};
public:
	addOp() {};
	addOp(int _x, int _y): x(_x), y(_y) {};
	~addOp() {};
	int operator()() { return x+y; };
};

BOOST_CLASS_EXPORT(addOp);

class wrapper {
private:
  friend class boost::serialization::access;
	async_task *task;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & task;
	};
public:
	wrapper() {};
	wrapper(async_task *_task) : task(_task) {};
	~wrapper(){};
	int operator()(){ return (*task)(); };
};

int helloWorld() {
	int id = Futures_Enviroment::Instance()->get_procId();
	cout << "- Worker" << id << ":Hello Master" << endl;
	return id;
}

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	
	Future<int> *message = async<int>(helloWorld);


	if(id == MASTER) {
		cout << "- Master :Hello " << message->get() << endl;
	}
/*
 	mpi::environment env(argc, argv);
	mpi::communicator world;
	int id = world.rank();
	if(id == 0) {
		async_task *add = new addOp(42, 1);
		wrapper w1(add);
		world.send(1, 0, w1);
	}
	else if(id == 1) {
		wrapper w2;
		world.recv( 0, 0, w2);
		cout << "result:" << w2() << endl;
	}
*/	
//	MPI_Barrier(MPI_COMM_WORLD);
	delete message;
	delete env;
}

