
#include "futures.hpp"
#include <iostream>

#define MASTER 0

using namespace std;
using namespace futures;

class fib {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	fib() {};
	~fib() {};
	int	operator()(int n) {
     if(n == 0) return 0;
     if(n == 1) return 1;
		 fib f1;
		 fib f2;
		 Future<int> *fib1 = async(f1, n-1);
		 Future<int> *fib2 = async(f2, n-2);
     int result = fib1->get() + fib2->get();
		 delete fib1;
		 delete fib2;
		 return result;
	};

};

typedef _async_stub<fib, int> fib_int;
BOOST_CLASS_EXPORT(fib_int);

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	
	fib f = fib();
	Future<int> *result = async(f, 10);


	if(id == MASTER) {
		cout << "- Master :You say " <<result->get()<< "?" << endl;
	}
	delete result;
	env->Finalize();
};

