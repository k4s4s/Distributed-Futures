
#include "futures.hpp"
#include <iostream>
#include <string>
#include <cereal/types/string.hpp>

#define MASTER 0

using namespace std;
using namespace futures;

class ping {
public:
	ping() {};
	~ping() {};
	string operator()(string message) { 
		//cout << "PING!" << endl;
		message.append(" PONG!");
		return message;
	};
};

FUTURES_SERIALIZE_CLASS(ping);
FUTURES_EXPORT_FUNCTOR((async_function<ping, string>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	ping p;

	REGISTER_TIMER("PING TIME");
	START_TIMER("PING TIME");
	future<string> pong = async(p, string("PING"));

	string message = pong.get();
	STOP_TIMER("PING TIME");
	cout << message << endl;
	PRINT_TIMER("PING TIME");
	Futures_Finalize();
};

