
#include "futures.hpp"
#include <iostream>

#define MASTER 0

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

	int a, n;

	while ((c = getopt(argc, argv, "a:n:")) != -1)
	switch (c)	{
		case 'a':
			a = atoi(optarg);	 
		 	break;		
		case 'n':
			n = atoi(optarg);	 
		 	break;

	 	default:
			cout << "Master:No value for which to calculate the fibonacci number was given" << endl;
			break;		
	}
	
	power f;
	future<int> message = async(f, a, n);

	cout << "- Master :Result is " <<message.get() << endl;
	
	Futures_Finalize();
};

