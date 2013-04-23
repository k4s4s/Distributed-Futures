
#ifndef _STATMANAGER_H
#define _STATMANAGER_H

#include <sys/time.h>
#include <map>
#include <string>
#include "../communication/communication.hpp"

/*
#define timer(timer_n) stats::StatManager::Instance()->register_timer(timer_n)
#define start_timer(timer_n) stats::StatManager::Instance()->start_timer(timer_n)
#define stop_timer(timer_n) stats::StatManager::Instance()->stop_timer(timer_n)
#define print_timer(timer_n) stats::StatManager::Instance()->print_timer(timer_n)
*/

#ifndef STATS

#define REGISTER_TIMER(timer_n)
#define REGISTER_COUNTER(counter_n);
#define START_TIMER(timer_n)
#define STOP_TIMER(timer_n)
#define INCREASE_COUNTER(counter_n, val)
#define DECREASE_COUNTER(counter_n, val)
#define PRINT_TIMER(timer_n)
#define PRINT_COUNTER(counter_n)
#define PRINT_STATS()

#else

#define REGISTER_TIMER(timer_n) futures::stats::StatManager::Instance()->register_timer(timer_n)
#define REGISTER_COUNTER(counter_n) futures::stats::StatManager::Instance()->register_counter(counter_n)
#define START_TIMER(timer_n) futures::stats::StatManager::Instance()->start_timer(timer_n)
#define STOP_TIMER(timer_n) futures::stats::StatManager::Instance()->stop_timer(timer_n)
#define INCREASE_COUNTER(counter_n, val) futures::stats::StatManager::Instance()->increase_counter(counter_n, val)
#define DECREASE_COUNTER(counter_n, val) futures::stats::StatManager::Instance()->decrease_counter(counter_n, val) 
#define PRINT_TIMER(timer_n) futures::stats::StatManager::Instance()->print_timer(timer_n)
#define PRINT_COUNTER(counter_n) futures::stats::StatManager::Instance()->print_counter(counter_n)
#define PRINT_STATS() futures::stats::StatManager::Instance()->print_stats()

#endif

#define FUTURES_REGISTER_TIMER(timer_n) futures::stats::StatManager::Instance()->register_timer(timer_n)
#define FUTURES_START_TIMER(timer_n) futures::stats::StatManager::Instance()->start_timer(timer_n)
#define FUTURES_STOP_TIMER(timer_n) futures::stats::StatManager::Instance()->stop_timer(timer_n)
#define FUTURES_PRINT_TIMER(timer_n) futures::stats::StatManager::Instance()->print_timer(timer_n)

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
		std::map<std::string, std::size_t> counterMap;
public:
    ~StatManager();
    static StatManager* Instance();
		void register_timer(std::string const& timer_n);
		void register_counter(std::string const& counter_n);
		void start_timer(std::string const& timer_n);
		void stop_timer(std::string const& timer_n);
		void increase_counter(std::string const& timer_n, std::size_t value);
		void decrease_counter(std::string const& timer_n, std::size_t value);
		unsigned long get_time(std::string const& timer_n);
		std::size_t get_count(std::string const& counter_n);
		void print_timer(std::string const& timer_n);
		void print_counter(std::string const& counter_n);
		void print_stats();
};

}//end of stats namespace
}//end of futures namespace

#endif

