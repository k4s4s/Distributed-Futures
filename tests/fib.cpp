
#include "futures.hpp"
#include <iostream>
#define MASTER 0

using namespace std;
using namespace futures;

class fib {
public:
	fib() {};
	~fib() {};
	int	operator()(int n) {
     if(n == 0) return 0;
     if(n == 1) return 1;
		 fib f1;
		 fib f2;
		 future<int> fib1 = async(f1, n-1);
		 future<int> fib2 = async(f2, n-2);
     int result = fib1.get() + fib2.get();
		 return result;
	};

};

FUTURES_EXPORT_FUNCTOR((async_function<fib, int>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	
	fib f = fib();
	future<int> result = async(f, 3);

	cout << "- Master :You say " <<result.get()<< "?" << endl;

	Futures_Finalize();
	cout << "done!";
};

