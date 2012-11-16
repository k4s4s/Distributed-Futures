#include <mpi.h>
#include <iostream>
#include "futures_enviroment.hpp"
#include "communication/communication.hpp"
#include "future.hpp"

using namespace futures;
int main(int argc, char* argv[]) {
	 int size;
   MPI_Comm parent; 
	 Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI");
	 //setup sharedData
   MPI_Comm_get_parent(&parent); 
	 std::cout << "Spawn: " << argv[1] << std::endl;
   /* 
    * Parallel code here.  
    * The manager is represented as the process with rank 0 in (the remote 
    * group of) MPI_COMM_PARENT.  If the workers need to communicate among 
    * themselves, they can use MPI_COMM_WORLD. 
    */ 
 
   delete env;
   return 0; 
}
