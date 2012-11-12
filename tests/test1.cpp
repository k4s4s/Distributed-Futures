
#include <mpi.h>
#include <boost/mpi/communicator.hpp>

#include "futures_enviroment.hpp"
#include "future.hpp"
#include "promise.hpp"
#include "mpi_comm.hpp"

#include <iostream>
#include <vector>

#define MASTER 0
#define MAX_FUTURES 1000
#define NUMBER_OF_FUTURES 1
#define SIZE_X 10
#define SIZE_Y 100

using namespace std;
//using namespace boost::mpi;
using namespace futures;

class Foo {
	public:
		int operator()() {
			return 42;
		};
};

int add(int a) {
	return 42+a;
}

int main(int argc, char* argv[]) {
	int id;
	Futures_Enviroment* env = Futures_Enviroment::Instance(argc, argv, "MPI");
	// we use boost to get the serilization functionality 
	boost::mpi::communicator comm = boost::mpi::communicator(MPI_COMM_WORLD,	boost::mpi::comm_attach); 	
	id = comm.rank();	
	double A[SIZE_X];

//	vector<Promise<double*>* > promises(NUMBER_OF_FUTURES);
//	vector<Future<double*>* > answers(NUMBER_OF_FUTURES);
//	Promise<int> int_promise(1, 0, 1, sizeof(int));
//	Future<int> *int_future = int_promise.get_future();
//	Foo foo;
	Future<int> *foo_fut = async<int>(add, 1, 0, 1, sizeof(int), MPI_INT, 10);
	cout << "Moving along #" << id << endl;

//	for(int i=0; i < NUMBER_OF_FUTURES; i++) {
//		promises[i] = new Promise<double*>(1, 0, SIZE_X, sizeof(double));
//		answers[i] = promises[i]->get_future();
//	}	

	if(id == MASTER) {
		cout << "Master waits for answer" << endl;
//		for(int i=0; i < NUMBER_OF_FUTURES; i++) {
//			double *B = answers[i]->get(MPI_DOUBLE);
//			for(int j = 0; j < SIZE_X; j++) {
//				cout << "The Answer to Life is " << B[j] << endl;
//			}
//		}
//		cout << "Int value is " << int_future->get(MPI_INT) << endl;
		cout << "foo returns " << foo_fut->get(MPI_INT) << endl;
	}
	else {
		cout << "Worker computes answer" << endl;
//		for(int i=0; i < SIZE_X; i++) {
//			A[i] = i;
//		}	

//		for(int i=0; i < NUMBER_OF_FUTURES; i++) {
			//cout << "A:" << A[0] << endl;
//    	promises[i]->set_value(&(A[0]), MPI_DOUBLE);
//		}
//		int_promise.set_value(42, MPI_INT);
	}
	
//	for(int i=0; i < NUMBER_OF_FUTURES; i++) {
//		delete promises[i];
//		delete answers[i];
//	}
//	delete int_future;
	delete foo_fut;
	delete env;
}

