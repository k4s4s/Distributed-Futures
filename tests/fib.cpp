
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
		 fib f;
		 future<int> fib1 = async(f, n-1);
		 future<int> fib2 = async(f, n-2);
		 return fib1.get() + fib2.get();;
	};

};

FUTURES_EXPORT_FUNCTOR((async_function<fib, int>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	
	fib f = fib();
	future<int> result = async(f, 5);

	cout << "- Master :Result is " <<result.get() << endl;

	Futures_Finalize();
	cout << "done!";
};

