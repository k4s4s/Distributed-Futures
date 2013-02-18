
#include "futures.hpp"
#include <iostream>
#include <vector>
#include <boost/serialization/vector.hpp>


#define DEFAULT_ARG_SIZE 10
#define DEFAULT_ARG_NUMBER 1

using namespace std;
using namespace futures;

template<typename... Args>
class bench {
public:
	bench() {};
	~bench() {};
	int operator()(Args... args) { 
		return 1;
	};
};

//FUTURES_SERIALIZE_CLASS(bench<vector<int> >);
FUTURES_EXPORT_FUNCTOR((async_function<bench<vector<int> >, vector<int> >));

//FUTURES_SERIALIZE_CLASS((bench<vector<int>,vector<int> >));
FUTURES_EXPORT_FUNCTOR((async_function<bench<vector<int>, vector<int> >, 
												vector<int>,vector<int> >));

FUTURES_EXPORT_FUNCTOR((async_function<bench<vector<int>, vector<int>,vector<int>,vector<int> >, 
												vector<int>,vector<int>,vector<int>, vector<int> >));

FUTURES_EXPORT_FUNCTOR((async_function<bench<vector<int>,vector<int>,vector<int>,vector<int>,
																						 vector<int>,vector<int>,vector<int>,vector<int> >, 
												vector<int>,vector<int>,vector<int>,vector<int>,
												vector<int>,vector<int>,vector<int>,vector<int> >));

namespace boost {
namespace serialization {

	template<class Archive>
	void serialize(Archive & ar, bench<vector<int> > c, const unsigned int version) {};

	template<class Archive>
	void serialize(Archive & ar, bench<vector<int>,vector<int> > c, const unsigned int version) {};

	template<class Archive>
	void serialize(Archive & ar, bench<vector<int>,vector<int>,
																		vector<int>,vector<int>> c, const unsigned int version) {};

	template<class Archive>
	void serialize(Archive & ar, bench<vector<int>,vector<int>,
																		vector<int>,vector<int>,
																		vector<int>,vector<int>,
																		vector<int>,vector<int> > c, const unsigned int version) {};

}
}

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
	bench<vector<int> > f_1arg;
	bench<vector<int>,vector<int> > f_2args;
	bench<vector<int>,vector<int>,vector<int>,vector<int> > f_4args;
	bench<vector<int>,vector<int>,vector<int>,vector<int>,
				vector<int>,vector<int>,vector<int>,vector<int> > f_8args;

	switch (arg_number)	{
		case	1:
			res = async(f_1arg, arg1);
			res.get();	 
		 	break;
		case	2:
			res = async(f_2args, arg1, arg2);
			res.get();	 
		 	break;		
		case	4:
			res = async(f_4args, arg1, arg2, arg3, arg4);
			res.get();	 
		 	break;	
		case	8:
			res = async(f_8args, arg1, arg2, arg3, arg4, 
									arg5, arg6, arg7, arg8);
			res.get(); 
		 	break;	
	 	default:
			cout << "ERROR: argument number must be 1,2,4 or 8!" << endl;
			break;		
	}	


	Futures_Finalize();
};

