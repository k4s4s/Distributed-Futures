
#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>
#include <iostream>

#define REGISTER_TIMER(X)
#define START_TIMER(x) start_timer()
#define STOP_TIMER(x) stop_timer()
#define PRINT_TIMER(x) print_timer()

unsigned long _timer_h_G_totaltime = 0;
unsigned long _timer_h_G_starttime = 0;
unsigned long _timer_h_G_endtime = 0;

void start_timer() {
	struct timeval time;
	gettimeofday(&time, NULL);
	_timer_h_G_starttime = time.tv_usec + time.tv_sec * 1000000;
};

void stop_timer() {
	struct timeval time;
	gettimeofday(&time, NULL);
	_timer_h_G_endtime = time.tv_usec + time.tv_sec * 1000000;
	_timer_h_G_totaltime = _timer_h_G_endtime - _timer_h_G_starttime;
};

void print_timer() {
	std::cout << "total time:" << ((double)_timer_h_G_totaltime/1000.0) <<  "ms" << std::endl;
}

#endif
