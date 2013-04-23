
#include "futures.hpp"
#include <iostream>
#include <cstdlib>

#define MASTER 0
#define DEFAULT_A 1
#define DEFAULT_N 2

using namespace std;
using namespace futures;

class power {
public:
	power() {};
	~power() {};
	double	operator()(double a, double n) {
		if(a == 0)	{
    	return 0;
		}
    else if(n == 0)	{
        return 1;
    }
    else {
			power f;
			future<double> p = async(f, a, n-1);
    	return a*p.get();
    }
	};
};


FUTURES_SERIALIZE_CLASS(power);
FUTURES_EXPORT_FUNCTOR((async_function<power, double, double>));


int main(int argc, char* argv[]) {

	Futures_Initialize(argc, argv);

	double a = DEFAULT_A, n = DEFAULT_N;
	char c;

	while ((c = getopt(argc, argv, "a:n:")) != -1)
	switch (c)	{
		case 'a':
			a = atoi(optarg);	 
		 	break;		
		case 'n':
			n = atoi(optarg);	 
		 	break;
	 	default:
			break;		
	}
	REGISTER_TIMER("total time");
	START_TIMER("total time");
	power f;
	future<double> message = async(f, a, n);
	double res = message.get();
	STOP_TIMER("total time");
	cout << "- Master :Result is " << res << endl;
	PRINT_TIMER("total time");
	Futures_Finalize();
};

