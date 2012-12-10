
#include "futures.hpp"
#include <iostream>

#define MASTER 0

using namespace std;
using namespace futures;

class power {
private:
	int base;
	int exponent;
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & base & exponent;		
	};
public:
	power() {};
	power(int _base, int _exponent): base(_base), exponent(_exponent) {};
	~power() {};
	int operator()(int a, int n) { 
		int i;
		int result = 1;
		Futures_Enviroment* env = Futures_Enviroment::Instance();
		int id = env->get_procId();
		exponent = n;
		base = a;
		for(i=1; i<=exponent; i++)	{
			result = result * base;
		}
		cout << "- Worker#" << id << ":"<<base<<"^"<<exponent<<"="<<result<< endl;
		return result;
	};
};

class power_rec {
private:
	int base;
	int exponent;
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & base & exponent;		
	};
public:
	power_rec() {};
	power_rec(int _base, int _exponent): base(_base), exponent(_exponent) {};
	~power_rec() {};
	int	operator()(int a, int n) {
		if(a == 0)	{
    	return 0;
		}
    else if(n == 0)	{
        return 1;
    }
    else if (n>0)	{
			power_rec f = power_rec();
			Future<int> *p = async(f, a, n-1);
			int retval = a*p->get();
			delete p; 
    	return retval;
    }
    else	{
			power_rec f = power_rec();
			Future<int> *p = async(f, a, n+1);
			int retval = (1/a)*p->get();
			delete p;
      return retval;
    }
	};

};

typedef _async_stub<power, int, int> power_int;
BOOST_CLASS_EXPORT(power_int);
typedef _async_stub<power_rec, int, int> power_rec_int;
BOOST_CLASS_EXPORT(power_rec_int);

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	
	power_rec f = power_rec(2,2);
	Future<int> *message = async(f, 2, 4);


	if(id == MASTER) {
		cout << "- Master :You say " <<message->get()<< "?" << endl;
	}
	delete message;
	env->Finalize();
};

