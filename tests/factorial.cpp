
#include <futures.hpp>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace futures;

#define DEFAULT_N 1

class factorial {
public:
	factorial() {};
	~factorial() {};
	double operator()(double n) {
    if (0 >= n)
        return 1;
		factorial f;
		future<double> n1 = async(f, n-1);
		return n * n1.get();
	};
};

FUTURES_SERIALIZE_CLASS(factorial);
FUTURES_EXPORT_FUNCTOR((async_function<factorial, double>));

int main(int argc, char* argv[]) {
	
	Futures_Initialize(argc, argv);
	
	double n = DEFAULT_N;
	char c;

	while ((c = getopt(argc, argv, "n:")) != -1)
	switch (c)	{
		case 'n':
			n = atoi(optarg);	 
		 	break;
	 	default:
			break;		
	}

	factorial f;
	future<double> result = async(f, n);

	cout << "- Master:Result is " <<result.get() << endl;

	Futures_Finalize();

};

