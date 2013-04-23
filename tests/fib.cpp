
#include "futures.hpp"
#include <iostream>
#include <cstdlib>
#define MASTER 0
#define DEFAULT_A 1

using namespace std;
using namespace futures;

class fib {
private:
public:
	fib() {};
	~fib() {};
	double	operator()(double n) {
     if(n == 0) return 0;
     if(n == 1) return 1;
		 fib f;
		 future<double> fib1 = async(f, n-1);
		 future<double> fib2 = async(f, n-2);
		 return fib1.get() + fib2.get();;
	};

};

FUTURES_SERIALIZE_CLASS(fib);
FUTURES_EXPORT_FUNCTOR((async_function<fib, double>));

int main(int argc, char* argv[]) {
	
	Futures_Initialize(argc, argv);
	
	double a = DEFAULT_A;
	char c;

	while ((c = getopt(argc, argv, "a:")) != -1)
	switch (c)	{
		case 'a':
			a = atoi(optarg);	 
		 	break;
	 	default:
			break;		
	}

	REGISTER_TIMER("total time");
	START_TIMER("total time");
	fib f = fib();
	future<double> result = async(f, a);
	double res = result.get();
	STOP_TIMER("total time");
	cout << "- Master:Result is " << res << endl;
	PRINT_TIMER("total time");
	Futures_Finalize();

};

