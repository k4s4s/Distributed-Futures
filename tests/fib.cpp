
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
    double min_work_unit;
		double n;
public:
    fib() {};
    fib(double _min_work_unit, double _n): min_work_unit(_min_work_unit), n(_n) {};
    ~fib() {};
    double	operator()() {
//std::cout << min_work_unit << std::endl;
        if(n == 0) return 0;
        if(n == 1) return 1;
        if(n > min_work_unit) {
            fib f1(min_work_unit, n-1);
            fib f2(min_work_unit, n-2);
            future<double> fib1 = async(f1);
            future<double> fib2 = async(f2);
            return fib1.get() + fib2.get();
        }
        else {
            fib f1(min_work_unit, n-1);
            fib f2(min_work_unit, n-2);
            return f1() + f2();
        }
    };

    template<class Archive>
    void serialize(Archive & ar)
    {
        ar(min_work_unit, n);
    };

};

//FUTURES_EXPORT_FUNCTOR((async_function<fib, double>));
CEREAL_REGISTER_TYPE(async_function<fib>);

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
    fib f = fib(min_work_unit, a);
    future<double> result = async(f);
    double res = result.get();
    STOP_TIMER("total time");
    cout << "- Master:Result is " << res << endl;
    PRINT_TIMER("total time");
    Futures_Finalize();

};

