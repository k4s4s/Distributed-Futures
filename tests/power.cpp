
#include "futures.hpp"
#include <iostream>
#include <boost/serialization/export.hpp>

#include <utility>
#include <cstddef> // NULL

#include <boost/config.hpp>
#include <boost/static_assert.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/type_traits/is_polymorphic.hpp>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/bool.hpp>

#include <boost/serialization/extended_type_info.hpp> // for guid_defined only
#include <boost/serialization/static_warning.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/force_include.hpp>
#include <boost/serialization/singleton.hpp>

#include <boost/archive/detail/register_archive.hpp>

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

//FUTURES_EXPORT_FUNCTOR(power);
typedef _async_stub<power, int, int> power_arg2;
BOOST_CLASS_EXPORT(power_arg2);
typedef _async_stub<power_rec, int, int> power_rec_arg2;
BOOST_CLASS_EXPORT(power_rec_arg2);

int main(int argc, char* argv[]) {
	Futures_Enviroment* env = Futures_Enviroment::Initialize(argc, argv, "MPI", "RR");
	int id = env->get_procId();
	
	power_rec f = power_rec(2,2);
	Future<int> *message = async(f, 2, 4);


	if(id == MASTER) {
		cout << "- Master :You say " <<message->get()<< "?" << endl;
	}
	delete message;
	delete env;
};

