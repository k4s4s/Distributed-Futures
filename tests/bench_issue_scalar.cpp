
#include "futures.hpp"
#include <iostream>
#include <vector>
#include <boost/serialization/vector.hpp>

#define DEFAULT_ARG_NUMBER 1

using namespace std;
using namespace futures;

typedef int type;

class bench1 {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
		ar & v1;
  }
	type v1;
public:
	bench1() {};
	bench1(type _v1): v1(_v1) {};
	~bench1() {};
	int operator()() { 
		return 1;
	};
};

class bench2 {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
		ar & v1;
		ar & v2;
  }
	type v1;
	type v2;
public:
	bench2() {};
	bench2(type _v1, type _v2):
	v1(_v1), v2(_v2) {};
	~bench2() {};
	int operator()() { 
		return 1;
	};
};

class bench4 {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
		ar & v1;
		ar & v2;
		ar & v3;
		ar & v4;
  }
	type v1;
	type v2;
	type v3;
	type v4;
public:
	bench4() {};
	bench4(type _v1, type _v2, type _v3, type _v4):
	v1(_v1), v2(_v2), v3(_v3), v4(_v4) {};
	~bench4() {};
	int operator()() { 
		return 1;
	};
};

class bench6 {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
		ar & v1;
		ar & v2;
		ar & v3;
		ar & v4;
		ar & v5;
		ar & v6;
  }
	type v1;
	type v2;
	type v3;
	type v4;
	type v5;
	type v6;
public:
	bench6() {};
	bench6(	type _v1, type _v2, type _v3, type _v4,
					type _v5, type _v6):
	v1(_v1), v2(_v2), v3(_v3), v4(_v4), v5(_v5), v6(_v6) {};
	~bench6() {};
	int operator()() { 
		return 1;
	};
};

class bench8 {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
		ar & v1;
		ar & v2;
		ar & v3;
		ar & v4;
		ar & v5;
		ar & v6;
		ar & v7;
		ar & v8;
  }
	type v1;
	type v2;
	type v3;
	type v4;
	type v5;
	type v6;
	type v7;
	type v8;
public:
	bench8 () {};
	bench8(	type _v1, type _v2, type _v3, type _v4,
					type _v5, type _v6, type _v7, type _v8):
	v1(_v1), v2(_v2), v3(_v3), v4(_v4), v5(_v5), v6(_v6), v7(_v7), v8(_v8) {};
	~bench8() {};
	int operator()() { 
		return 1;
	};
};

FUTURES_EXPORT_FUNCTOR((async_function<bench1>));

FUTURES_EXPORT_FUNCTOR((async_function<bench2>));

FUTURES_EXPORT_FUNCTOR((async_function<bench4>));

FUTURES_EXPORT_FUNCTOR((async_function<bench6>));

FUTURES_EXPORT_FUNCTOR((async_function<bench8>));

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	
	int arg_number = DEFAULT_ARG_NUMBER;
	char c;

	while ((c = getopt(argc, argv, "a:n:")) != -1)
	switch (c)	{
		case 'a':
			arg_number = atoi(optarg);	 
			break;	
	 	default:
			break;		
	}

	type arg1;
	type arg2;
	type arg3;
	type arg4;
	type arg5;
	type arg6;
	type arg7;
	type arg8;

	future<int> res;
	bench1 f_1arg(arg1);
	bench2 f_2args(arg1, arg2);
	bench4 f_4args(arg1, arg2, arg3, arg4);
	bench6 f_6args(arg1, arg2, arg3, arg4, arg5, arg6);
	bench8 f_8args(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);

	REGISTER_TIMER("total time");
	START_TIMER("total time");
	switch (arg_number)	{
		case	1:
			res = async(f_1arg);
			res.get();	 
		 	break;
		case	2:
			res = async(f_2args);
			res.get();	 
		 	break;		
		case	4:
			res = async(f_4args);
			res.get();	 
		 	break;	
		case	6:
			res = async(f_6args);
			res.get();	 
		 	break;	
		case	8:
			res = async(f_8args);
			res.get(); 
		 	break;	
	 	default:
			cout << "ERROR: argument number must be 1,2,4,6 or 8!" << endl;
			break;		
	}
	STOP_TIMER("total time");	
	PRINT_TIMER("total time");
	Futures_Finalize();
};

