
#include "futures.hpp"
#include <iostream>

#define MASTER 0

using namespace std;
using namespace futures;

class ping {
public:
	ping() {};
	~ping() {};
	int operator()() { 
		cout << "PING!" << endl;
		return 1;
	};
};

FUTURES_SERIALIZE_CLASS(ping);
FUTURES_EXPORT_FUNCTOR((async_function<ping>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	ping p;

	REGISTER_TIMER("PING TIME");
	START_TIMER("PING TIME");
	future<int> pong = async(p);

	pong.get();
	STOP_TIMER("PING TIME");
	cout << "PONG!" << endl;
	PRINT_TIMER("PING TIME");
	Futures_Finalize();
};

