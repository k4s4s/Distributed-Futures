#include "futures.hpp"
#include <iostream>
#include <cstdlib>
#define MASTER 0
#define DEFAULT_A 1
#define DEFAULT_MIN_WORK_UNIT 1

using namespace std;
using namespace futures;

class fib {
private:
  friend class boost::serialization::access;
template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & min_work_unit;
}
double min_work_unit;
public:
fib() {};
fib(double _min_work_unit): min_work_unit(_min_work_unit) {};
~fib() {};
double	operator()(double n) {
//std::cout << min_work_unit << std::endl;
if(n == 0) return 0;
if(n == 1) return 1;
if(n > min_work_unit) {
fib f(min_work_unit);
future<double> fib1 = async(f, n-1);
future<double> fib2 = async(f, n-2);
return fib1.get() + fib2.get();
}
else {
fib f(min_work_unit);
return f(n-1) + f(n-2);
}
};

};

FUTURES_EXPORT_FUNCTOR((async_function<fib, double>));

int main(int argc, char* argv[]) {

Futures_Initialize(argc, argv);

double a = DEFAULT_A;
double min_work_unit = DEFAULT_MIN_WORK_UNIT;
char c;

while ((c = getopt(argc, argv, "a:w:")) != -1)
switch (c)	{
case 'a':
a = atoi(optarg);	
break;
case 'w':
min_work_unit = atoi(optarg);	
break;
default:
break;	
}

REGISTER_TIMER("total time");
START_TIMER("total time");
fib f = fib(min_work_unit);
future<double> result = async(f, a);
double res = result.get();
STOP_TIMER("total time");
cout << "- Master:Result is " << res << endl;
PRINT_TIMER("total time");
Futures_Finalize();

};

