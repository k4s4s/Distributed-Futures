
#include "futures.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#define MASTER 0

using namespace std;
using namespace futures;

class foo {
public:
	foo() {};
	~foo() {};
	int operator()() { 
		int id = Futures_Id();
		std::cout << "Runnig job on " << id << endl;
		return 42;
	};
};

FUTURES_SERIALIZE_CLASS(foo);
FUTURES_EXPORT_FUNCTOR((async_function<foo>));

class goo {
public:
	goo() {};
	~goo() {};
	int operator()(future<int> msg) { 
		int id = Futures_Id();
		std::cout << "Waiting for future on " << id << endl;
		return msg.get();
	};
};

FUTURES_SERIALIZE_CLASS(goo);
FUTURES_EXPORT_FUNCTOR((async_function<goo, future<int>>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	foo f;
	goo g;
	future<int> fut = async(f);
	future<int> message = async(g, fut);

	cout << "- Master : " << message.get() << endl;

	Futures_Finalize();
};

