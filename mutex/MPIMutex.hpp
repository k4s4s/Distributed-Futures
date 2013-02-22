
#ifndef _MPIMUTEX_H
#define _MPIMUTEX_H

#include <mutex.hpp>
#include <mpi.h>

typedef unsigned char byte;

class MPIMutex : public mutex {
private:
	MPI_Win win;
	MPI_Comm comm;
	byte *lock_vector;
	unsigned int id; 
public:
	MPIMutex(MPI_Comm _comm);
	~MPIMutex();
	bool try_lock(int proc);
	void lock(int proc);
	void unlock(int proc);
};

#endif
