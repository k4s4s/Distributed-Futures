
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
			int retval = a*p.get();
    	return retval;
    }
    else	{
			power f;
			future<int> p = async(f, a, n+1);
			int retval = (1/a)*p.get();
      return retval;
    }
	};
};

FUTURES_EXPORT_FUNCTOR((async_function<power, int, int>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	
	power f;
	future<int> message = async(f, 2, 4);

	cout << "- Master :You say " <<message.get()<< "?" << endl;
	
	Futures_Finalize();
};

