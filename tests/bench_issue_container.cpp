
#include "futures.hpp"
#include <iostream>
#include <vector>
#include <boost/serialization/vector.hpp>


#define DEFAULT_ARG_SIZE 10
#define DEFAULT_ARG_NUMBER 1

using namespace std;
using namespace futures;

class bench1 {
private:
  friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
		ar & v1;
  }
	vector<int> v1;
public:
	bench1() {};
	bench1(vector<int> _v1): v1(_v1) {};
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
	vector<int> v1;
	vector<int> v2;
public:
	bench2() {};
	bench2(vector<int> _v1, vector<int> _v2):
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
	vector<int> v1;
	vector<int> v2;
	vector<int> v3;
	vector<int> v4;
public:
	bench4() {};
	bench4(vector<int> _v1, vector<int> _v2, vector<int> _v3, vector<int> _v4):
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
	vector<int> v1;
	vector<int> v2;
	vector<int> v3;
	vector<int> v4;
	vector<int> v5;
	vector<int> v6;
public:
	bench6() {};
	bench6(	vector<int> _v1, vector<int> _v2, vector<int> _v3, vector<int> _v4,
					vector<int> _v5, vector<int> _v6):
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
	vector<int> v1;
	vector<int> v2;
	vector<int> v3;
	vector<int> v4;
	vector<int> v5;
	vector<int> v6;
	vector<int> v7;
	vector<int> v8;
public:
	bench8 () {};
	bench8(	vector<int> _v1, vector<int> _v2, vector<int> _v3, vector<int> _v4,
					vector<int> _v5, vector<int> _v6, vector<int> _v7, vector<int> _v8):
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

/*
FUTURES_SERIALIZE_CLASS(bench<vector<int>,vector<int>,vector<int>,vector<int> >);

FUTURES_SERIALIZE_CLASS(bench<vector<int>,vector<int>,vector<int>,vector<int>,
															vector<int>,vector<int>,vector<int>,vector<int> >);
*/

int main(int argc, char* argv[]) {
	Futures_Initialize(argc, argv);
	
	int arg_size = DEFAULT_ARG_SIZE;
	int arg_number = DEFAULT_ARG_NUMBER;
	char c;

	while ((c = getopt(argc, argv, "a:n:")) != -1)
	switch (c)	{
		case 'a':
			arg_number = atoi(optarg);	 
		 	break;
		case 'n':
			arg_size = atoi(optarg);	 
		 	break;		
	 	default:
			break;		
	}

	vector<int> arg1(arg_size);
	vector<int> arg2(arg_size);
	vector<int> arg3(arg_size);
	vector<int> arg4(arg_size);
	vector<int> arg5(arg_size);
	vector<int> arg6(arg_size);
	vector<int> arg7(arg_size);
	vector<int> arg8(arg_size);

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

