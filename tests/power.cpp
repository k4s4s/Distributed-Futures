
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
	int	operator()(int a, int n) {
		if(a == 0)	{
    	return 0;
		}
    else if(n == 0)	{
        return 1;
    }
    else {
			power f;
			future<int> p = async(f, a, n-1);
    	return a*p.get();
    }
	};
};


FUTURES_SERIALIZE_CLASS(power);
FUTURES_EXPORT_FUNCTOR((async_function<power, int, int>));


int main(int argc, char* argv[]) {

	Futures_Initialize(argc, argv);

	int a = DEFAULT_A, n = DEFAULT_N;
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
	
	power f;
	future<int> message = async(f, a, n);

	cout << "- Master :Result is " <<message.get() << endl;
	
	Futures_Finalize();
};

