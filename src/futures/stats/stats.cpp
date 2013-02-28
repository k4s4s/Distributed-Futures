
#include "stats.hpp"
#include <mpi.h>

using namespace futures::stats;
using namespace std;

/*** _timer class implementation ***/
_timer::_timer() {
	_timer_h_G_totaltime = 0;
	_timer_h_G_starttime = 0;
	_timer_h_G_endtime = 0;
};

_timer::~_timer() {};

void _timer::start_timer() {
	struct timeval time;
	gettimeofday(&time, NULL);
	_timer_h_G_starttime = time.tv_usec + time.tv_sec * 1000000;
};

void _timer::stop_timer() {
	struct timeval time;
	gettimeofday(&time, NULL);
	_timer_h_G_endtime = time.tv_usec + time.tv_sec * 1000000;
	_timer_h_G_totaltime = _timer_h_G_endtime - _timer_h_G_starttime;
};

unsigned long _timer::get_time() {
	return _timer_h_G_totaltime;
};

/*** StatManager class implementation ***/
StatManager* StatManager::pinstance;

StatManager::StatManager() {
	timerMap["initialization_time"] = _timer();
	timerMap["job_issue_time"] = _timer();
	timerMap["job_execution_time"] = _timer();
	timerMap["idle_time"] = _timer();
	timerMap["finalization_time"] = _timer();
	timerMap["total_execution_time"] = _timer();
	counterMap["total_jobs"] = 0;
	counterMap["total_memory_needed"] = 0;
};

StatManager::~StatManager() {
	timerMap.clear();
	counterMap.clear();
};

StatManager* StatManager::Instance() {
	if(pinstance == NULL) {
		pinstance = new StatManager();
	}
	return pinstance;
};
 
void StatManager::start_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = timerMap.find(timer_n);
	it->second.start_timer();
};

void StatManager::stop_timer(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = timerMap.find(timer_n);
	it->second.stop_timer();
};

void StatManager::increase_counter(std::string const& counter_n, std::size_t value) {
	map<string, std::size_t>::iterator it;
	it = counterMap.find(counter_n);
	it->second += value;
};

void StatManager::decrease_counter(std::string const& counter_n, std::size_t value) {
	map<string, std::size_t>::iterator it;
	it = counterMap.find(counter_n);
	it->second -= value;
};

void StatManager::register_timer(std::string const& timer_n) {
	timerMap[timer_n] = _timer();
}

void StatManager::register_counter(std::string const& counter_n) {
	counterMap[counter_n] = 0;
}

unsigned long StatManager::get_time(std::string const& timer_n) {
	map<string, _timer>::iterator it;
	it = timerMap.find(timer_n);
	return it->second.get_time();
};

std::size_t StatManager::get_count(std::string const& counter_n) {
	map<string, std::size_t>::iterator it;
	it = counterMap.find(counter_n);
	return it->second;
};

void StatManager::print_timer(std::string const& timer_n) {
	cout << timer_n<<":"<< ((double)this->get_time(timer_n)/1000.0) << "ms" << endl;	
};

void StatManager::print_counter(std::string const& counter_n) {
	cout << counter_n<<":"<< this->get_count(counter_n) << endl;	
};

void StatManager::print_stats() {
	int nprocs, id;
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	//FIXME: master should collect statistics over all workers
	unsigned long total_job_issue_time = this->get_time("job_issue_time") ;
	long total_total_jobs = this->get_count("total_jobs"); //terrible name for a variable :P
	unsigned long total_idle_time = this->get_time("idle_time");
	if(id == 0) {

			
		long jobs_num = 0;
		for(int i = 1; i < nprocs; i++) {
				MPI_Status status;
				int go;
				MPI_Send(&go, 1, MPI_BYTE, i, 0, MPI_COMM_WORLD);
				MPI_Recv(&jobs_num, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				total_total_jobs += jobs_num;
		}

		cout << "==== FUTURE'S RUNTIME STATS ===" << endl;
		cout << "        total mpi processes:" << nprocs << endl;  	
		cout << "total number of jobs issued:" << total_total_jobs << endl;
	
		cout << "==== MASTER TIMING STATS ===" << endl;	 
		cout << "    total job issue time:" << ((double)this->get_time("job_issue_time")/1000.0) << "ms" << endl;
		cout << "  job issue time per job:" 
			<< ((double)this->get_time("job_issue_time")/1000.0)/((double)this->get_count("total_jobs")) 
			<< "ms" << endl;
		cout << "total job execution time:" << ((double)this->get_time("job_execution_time")/1000.0) << "ms" << endl;
		cout << "         total idle time:" << ((double)this->get_time("idle_time")/1000.0) << "ms" << endl;

		//here get worker stats
		for(int i = 1; i < nprocs; i++) {
				MPI_Status status;
				MPI_Recv(&jobs_num, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				unsigned long _job_issue_time = 0;	
				MPI_Recv(&_job_issue_time, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				total_job_issue_time += _job_issue_time;
				unsigned long _idle_time;
				MPI_Recv(&_idle_time, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				total_idle_time += _idle_time;
	
				cout << "==== Worker#"<<i<<" TIMING STATS ===" << endl;	 
				cout << "    total job issue time:" << ((double)_job_issue_time/1000.0) << "ms" << endl;
				cout << "  job issue time per job:" << ((double)_job_issue_time/1000.0)/((double)jobs_num)<< "ms" << endl;
				cout << "total job execution time:" << ((double)this->get_time("job_execution_time")/1000.0) << "ms" << endl;
				cout << "         total idle time:" << ((double)_idle_time/1000.0) << "ms" << endl;	
				cout << "			total memory needed:" << this->get_count("total_memory_needed") << "bytes" << endl;
		}
		double total_actual_execution_time = this->get_time("total_execution_time")
			-this->get_time("initialization_time")-this->get_time("finalization_time");
		cout << "==== TOTAL TIMING STATS ===" << endl;	 
		cout << "     initialization time:" << ((double)this->get_time("initialization_time")/1000.0) << "ms" << endl;
		cout << "    total job issue time:" << ((double)total_job_issue_time/1000.0) << "ms" << endl;
		cout << "  job issue time per job:" << ((double)total_job_issue_time/1000.0)/
																					((double)total_total_jobs) << "ms" << endl;
		cout << "total job execution time:" << ((double)this->get_time("job_execution_time")/1000.0) << "ms" << endl;
		cout << "         total idle time:" << ((double)total_idle_time/1000.0) << "ms" << endl;
		cout << "       finalization time:" << ((double)this->get_time("finalization_time")/1000.0) << "ms" << endl;
		cout << "    total execution time:" << ((double)this->get_time("total_execution_time")/1000.0) << "ms" << endl;
		cout << "   actual execution time:" << ((double)total_actual_execution_time/1000.0) << "ms" << endl;
	}
	else {
		MPI_Status status;
		int go;
		MPI_Recv(&go, 1, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
		long total_jobs = this->get_count("total_jobs");
		MPI_Send(&total_jobs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		total_jobs = this->get_count("total_jobs");
		MPI_Send(&total_jobs, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		unsigned long job_issue_time = this->get_time("job_issue_time");
		MPI_Send(&job_issue_time, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		unsigned long idle_time = this->get_time("idle_time");
		MPI_Send(&idle_time, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);		
	}
};

