
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
    else if (n>0)	{
			power f;
			future<int> p = async(f, a, n-1);
    	return a*p.get();
    }
    else	{
			power f;
			future<int> p = async(f, a, n+1);
      return (1/a)*p.get();
    }
	};
};

FUTURES_EXPORT_FUNCTOR((async_function<power, int, int>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	
	power f;
	future<int> message = async(f, 2, 10);

	cout << "- Master :Result is " <<message.get() << endl;
	
	Futures_Finalize();
};

