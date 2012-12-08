
#include "futures.hpp"
#include <iostream>


#define MASTER 0

using namespace std;
using namespace futures;

class helloWorld {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	helloWorld() {};
	~helloWorld() {};
	int operator()() { 
		int id = Futures_Enviroment::Instance()->get_procId();
		cout << "- Worker" << id << ":Hello Master" << endl;
		return id;
	};
};

int helloWorld_func() { 
	int id = Futures_Enviroment::Instance()->get_procId();
	cout << "- Worker" << id << ":Hello Master" << endl;
	return id;
};


template<typename ... Types>
class simple_tuple 
{
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	simple_tuple() {};
};

template<>
class simple_tuple<>
{};

template<typename First,typename ... Rest>
class simple_tuple<First,Rest...>: private simple_tuple<Rest...>
{
    First member;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & boost::serialization::base_object<_stub>(*this);
				ar & member;
    };
public:
    simple_tuple(First const& f,Rest const& ... rest):
        simple_tuple<Rest...>(rest...),
        member(f)
    {};
};

FUTURES_EXPORT_FUNCTOR(helloWorld);

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	simple_tuple<int, int> t(1,1);	
	helloWorld f;
	Future<int> *message = async(f);


	if(id == MASTER) {
		cout << "- Master :Hello " << message->get() << endl;
	}
	delete message;
	delete env;
};

