
#ifndef _STATMANAGER_H
#define _STATMANAGER_H

#include <sys/time.h>
#include <map>
#include <string>

namespace futures {
namespace stats {

class _timer {
private:
	unsigned long _timer_h_G_starttime;
	unsigned long _timer_h_G_endtime;
	unsigned long _timer_h_G_totaltime;
public:
		_timer();
		~_timer();
		void start_timer();
		void stop_timer();
		unsigned long get_time();
};

class StatManager {
private:
    StatManager();
    static StatManager *pinstance;
		std::map<std::string, _timer> timerMap;
		unsigned long total_jobs;
public:
    ~StatManager();
    static StatManager* Instance();
		void start_timer(std::string const& timer_n);
		void stop_timer(std::string const& timer_n);
		unsigned long get_time(std::string const& timer_n);
		void print_stats();
		void increase_total_jobs();
};

}//end of stats namespace
}//end of futures namespace

#endif

