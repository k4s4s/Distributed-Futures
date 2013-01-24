
#include "futures.hpp"
#include <iostream>
#include <cstdlib>
#define MASTER 0

using namespace std;
using namespace futures;

class fib {
private:
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

FUTURES_SERIALIZE_CLASS(fib);
FUTURES_EXPORT_FUNCTOR((async_function<fib, int>));

int main(int argc, char* argv[]) {
	
	Futures_Initialize(argc, argv);
	
	int a;
	char c;

	while ((c = getopt(argc, argv, "a:")) != -1)
	switch (c)	{
		case 'a':
			a = atoi(optarg);	 
		 	break;
	 	default:
			cout << "Master:No value for which to calculate the fibonacci number was given" << endl;
			break;		
	}

	fib f = fib();
	future<int> result = async(f, a);

	cout << "- Master:Result is " <<result.get() << endl;

	Futures_Finalize();
	cout << "done!";
};

