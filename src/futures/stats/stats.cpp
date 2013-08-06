
#include "stats.hpp"
#include <mpi.h>
#include <cstring>
#include <iomanip>
#include <cassert>

#define l_margin 30

using namespace futures::stats;
using namespace std;

/*** _timer class implementation ***/
_timer::_timer() {
	_timer_h_G_totaltime = 0;
	_timer_h_G_starttime = 0;
	_timer_h_G_endtime = 0;
	running = false;
};

_timer::~_timer() {};

void _timer::start_timer() {
/*
	if(running) {
		std::cout << "timer was alread running" << std::endl; //FIXME:this should never happen, assert it maybe
		return;
	}
*/
	assert(!running);
	struct timeval time;
	gettimeofday(&time, NULL);
	_timer_h_G_starttime = time.tv_usec + time.tv_sec * 1000000;
	running = true;
};

void _timer::stop_timer() {
	if(!running) return; 
	struct timeval time;
	gettimeofday(&time, NULL);
	_timer_h_G_endtime = time.tv_usec + time.tv_sec * 1000000;
	_timer_h_G_totaltime += _timer_h_G_endtime - _timer_h_G_starttime;
	running = false;
};

unsigned long _timer::get_time() {
	return _timer_h_G_totaltime;
};

bool _timer::is_running() {
	return running;
};


/*** StatManager class implementation ***/
StatManager* StatManager::pinstance;

StatManager::StatManager() {
	sys_timerMap["initialization_time"] = _timer();
	sys_timerMap["finalization_time"] = _timer();
	sys_timerMap["job_issue_time"] = _timer();
	sys_timerMap["idle_time"] = _timer();
	sys_timerMap["total_execution_time"] = _timer();
	sys_timerMap["user_code_execution_time"] = _timer();
	sys_timerMap["value_return_time"] = _timer();
	sys_timerMap["find_available_worker_time"] = _timer();
	sys_timerMap["terminate_time"] = _timer();

	sys_counterMap["total_jobs"] = 0;
	sys_counterMap["total_memory_needed"] = 0;
};

StatManager::~StatManager() {
	sys_timerMap.clear();
	sys_counterMap.clear();
	usr_timerMap.clear();
};

StatManager* StatManager::Instance() {
	if(pinstance == NULL) {
		pinstance = new StatManager();
	}
	return pinstance;
};
 
void StatManager::start_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = sys_timerMap.find(timer_n);
	if(it->second.is_running()) std::cout << "timer " << timer_n << "is still running" << endl;
	it->second.start_timer();
};

void StatManager::stop_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = sys_timerMap.find(timer_n);
	it->second.stop_timer();
};

void StatManager::start_usr_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = usr_timerMap.find(timer_n);
	it->second.start_timer();
};

void StatManager::stop_usr_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = usr_timerMap.find(timer_n);
	it->second.stop_timer();
};

void StatManager::increase_counter(std::string const& counter_n, std::size_t value) {
	map<string, std::size_t>::iterator it;
	it = sys_counterMap.find(counter_n);
	it->second += value;
};

void StatManager::decrease_counter(std::string const& counter_n, std::size_t value) {
	map<string, std::size_t>::iterator it;
	it = sys_counterMap.find(counter_n);
	it->second -= value;
};

void StatManager::register_timer(std::string const& timer_n) {
	sys_timerMap[timer_n] = _timer();
};

void StatManager::register_usr_timer(std::string const& timer_n) {
	usr_timerMap[timer_n] = _timer();
};

void StatManager::register_counter(std::string const& counter_n) {
	sys_counterMap[counter_n] = 0;
};

unsigned long StatManager::get_time(std::string const& timer_n) {
	map<string, _timer>::iterator it; 
	it = sys_timerMap.find(timer_n);
	if(it == sys_timerMap.end()) return 0; //not found
	return it->second.get_time();
};

std::size_t StatManager::get_count(std::string const& counter_n) {
	map<string, std::size_t>::iterator it;
	it = sys_counterMap.find(counter_n);
	if(it == sys_counterMap.end()) return 0; //not found
 	return it->second;
};

void StatManager::print_timer(std::string const& timer_n) {
	cout << timer_n << ":" << ((double)this->get_time(timer_n)/1000.0) << "ms" << endl;	
};

void StatManager::print_usr_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it; 
	it = usr_timerMap.find(timer_n);
	if(it == usr_timerMap.end()) return; //not found
	cout << timer_n << ":" << ((double)(it->second.get_time())/1000.0) << "ms" << endl;	
};

void StatManager::print_counter(std::string const& counter_n) {
	cout << counter_n<< ":" << this->get_count(counter_n) << endl;	
};

void StatManager::print_stats(futures::communication::CommInterface *comm) {
	int nprocs, id;
	nprocs = comm->size();
	id = comm->get_procId();

	if(id == 0) {

		cout << "==== FUTURE'S RUNTIME STATS ====" << endl;
		cout << setw(l_margin) << "total_mpi_processes:" << nprocs << endl;  	
			
		//collect and sum counters
		map<string, std::size_t>::iterator it_c;
		char buff[100];
		for(it_c = sys_counterMap.begin(); it_c != sys_counterMap.end(); ++it_c) {
			//cout << it->first << ":" << ((double)it->second/1000.0) << "ms";
			std::size_t counter_sum = it_c->second;
			for(int i = 1; i < nprocs; i++) {
				string req = it_c->first;
				strcpy(buff, req.c_str());
				comm->send(i, 0, 100, MPI_CHAR, buff);
				std::size_t counter;
				comm->recv(i, 0, 1, MPI_LONG, &counter);
				counter_sum += counter;		
			}
			cout << setw(l_margin) << it_c->first << ":" << counter_sum << endl;			
		}
		//ok, send done to all workers
		for(int i = 1; i < nprocs; i++) {
			string req = "done";
			strcpy(buff, req.c_str());
			comm->send(i, 0, 100, MPI_CHAR, buff);
		}
		
		cout << "==== MASTER TIMING STATS ====" << endl;	

		map<string, _timer>::iterator it_t;
		for(it_t = sys_timerMap.begin(); it_t != sys_timerMap.end(); ++it_t) {
			cout << setw(l_margin) << it_t->first << ":" << ((double)it_t->second.get_time()/1000.0) << "ms" << endl;
		}

		for(int i = 1; i < nprocs; i++) {
			cout << "==== Worker#"<<i<<" TIMING STATS ====" << endl;	
		
			for(it_t = sys_timerMap.begin(); it_t != sys_timerMap.end(); ++it_t) {
					string req = it_t->first;
					strcpy(buff, req.c_str());
					comm->send(i, 0, 100, MPI_CHAR, buff);
					unsigned long time_value;
					comm->recv(i, 0, 1, MPI_UNSIGNED_LONG, &time_value);
					cout << setw(l_margin) << it_t->first << ":" << ((double)time_value/1000.0) << endl;			
			}
			//ok, send done to worker
			string req = "done";
			strcpy(buff, req.c_str());
			comm->send(i, 0, 100, MPI_CHAR, buff);
		}
	}
	else {
		//send counters to master
		while(1) {
			char buff[100];
			comm->recv(0, 0, 100, MPI_CHAR, &buff);
			string req = string(buff);
			if(req.compare("done") == 0) break;
			long counter = get_count(req);
			comm->send(0, 0, 1, MPI_LONG, &counter);
		}

		while(1) {
			char buff[100];
			comm->recv(0, 0, 100, MPI_CHAR, &buff);
			string req = string(buff);
			if(req.compare("done") == 0) break;
			unsigned long timer_value = get_time(req); 
			comm->send(0, 0, 1, MPI_UNSIGNED_LONG, &timer_value);
		}
	}
};


