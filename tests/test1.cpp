
#include <mpi.h>
#include <boost/mpi/communicator.hpp>

#include "futures_enviroment.hpp"
#include "future.hpp"
#include "promise.hpp"

#include <iostream>
#include <vector>

#define MASTER 0
#define MAX_FUTURES 1000
#define NUMBER_OF_FUTURES 1
#define SIZE_X 100
#define SIZE_Y 100

using namespace std;
//using namespace boost::mpi;

int main(int argc, char* argv[]) {
	int id;
	Futures_Enviroment* env = Futures_Enviroment::Instance(argc, argv, MAX_FUTURES);
	// we use boost to get the serilization functionality 
	boost::mpi::communicator comm = boost::mpi::communicator(env->get_communicator(),	boost::mpi::comm_attach); 	
	id = comm.rank();	
	double A[SIZE_X];
	if(id == MASTER) {
		cout << "Master creates promise and future" << endl;
		vector<Promise<double> > promises(NUMBER_OF_FUTURES);
		vector<Future<double> > answers(NUMBER_OF_FUTURES);
		Promise<char> sayP(id);
		Future<char> sayF = sayP.get_future();
		comm.send(1, 0, sayP);
		cout << sayF.get(MPI_CHAR) << "imitris" << endl;
		for(int i=0; i < NUMBER_OF_FUTURES; i++) {
			promises[i] = Promise<double>(id);
			answers[i] = promises[i].get_future();
			comm.send(1, 0, promises[i]);
		}		
		//Promise<double> promise(id);	
		//while(!answer_to_life.is_ready()); //spin until future is ready
		for(int i=0; i < NUMBER_OF_FUTURES; i++) {
#ifdef ARMCI_V
			double answ = answers[i].get();
			cout << "The Answer to Life is " << answ << endl;
#else
			cout << "The Answer to Life is " << answers[i].get(MPI_DOUBLE) << endl;
#endif
		}
	}
	else {
		for(int i=0; i < SIZE_X; i++) {
			A[i] = i+42;
		}	
		Promise<char> sayP;
		comm.recv(0, 0, sayP);
		vector<Promise<double> > promises(NUMBER_OF_FUTURES);
		sayP.set_value('D', MPI_CHAR);		
		for(int i=0; i < NUMBER_OF_FUTURES; i++) {
    	comm.recv(0, 0, promises[i]);
		}
		for(int i=0; i < NUMBER_OF_FUTURES; i++) {
#ifdef ARMCI_V
    	promises[i].set_value(A, SIZE_X*sizeof(double));
#else
    	promises[i].set_value(i+42, MPI_DOUBLE);
#endif
		}
	}
	
	/* array testing */
	/*
	double A[SIZE_X][SIZE_Y];
	for(int i=0; i < SIZE_X; i++) {
		for(int j=0; j < SIZE_Y; j++) {
			A[i][j] = i*SIZE_X+j;
		}
	} 

	for(int i=0; i < SIZE_X; i++) {
		for(int j=0; j < SIZE_Y; j++) {
			cout << "A["<<i<<"]["<<j<<"]=" << A[i][j] << endl;
		}
	}
	int i=0, j=1;
	cout << "A["<<i<<"]["<<j<<"]=" << A[i][j] << endl;
	cout << "A["<<i<<"*SIZE_Y+"<<j<<"]=" << A[i*SIZE_X+j] << endl;
	*/
	delete env;
}

