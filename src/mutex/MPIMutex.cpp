
#include <MPIMutex.hpp>

#include <stdlib.h>
#include <iostream>
#include <mpi.h>

/*mpi mutex algorithm copies MPICH2 implementation,
	ref:ispa12_tutorial.pdf and MPICH2 source code */
#define MPI_MUTEX_TAG 100

MPIMutex::MPIMutex(MPI_Comm _comm) {
	int nproc, rank;
	id = details::mutex_count++;
	comm = _comm;
	MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &nproc);
	MPI_Alloc_mem(nproc, MPI_INFO_NULL, &lock_vector);
	bzero(lock_vector, nproc);
	MPI_Win_create(lock_vector, nproc, sizeof(byte), MPI_INFO_NULL, comm, &win);
};

MPIMutex::~MPIMutex() {
	MPI_Win_free(&win);
	MPI_Free_mem(lock_vector);
};

bool MPIMutex::try_lock(int proc) {
	int rank, nproc, already_locked;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &nproc);

	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, proc, 0, win);
	byte *buff = (byte*)malloc(sizeof(byte)*nproc);
	buff[rank] = 1;
  MPI_Put(&(buff[rank]), 1, MPI_BYTE, proc, rank, 1, MPI_BYTE, win);
  /* Get data to the left of rank */
  if (rank > 0) {
      MPI_Get(buff, rank, MPI_BYTE, proc, 0, rank, MPI_BYTE, win);
  }
  /* Get data to the right of rank */
  if (rank < nproc - 1) {
      MPI_Get(&(buff[rank+1]), nproc-1-rank, MPI_BYTE, proc, rank+1, nproc-1-rank,
              MPI_BYTE, win);
  }
  MPI_Win_unlock(proc, win);
	/* check if anyone has the lock*/
	already_locked = 1; //1 means succesfully got the lock
	for (int i = 0; i < nproc; i++)
    if (buff[i] && i != rank)
        already_locked = 0;

	free(buff);
	return already_locked;	
};
	

void MPIMutex::lock(int proc) {
	int rank, nproc, already_locked;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &nproc);
	//std::cout << "trying to get lock" << std::endl;
	MPI_Win_lock(MPI_LOCK_EXCLUSIVE, proc, 0, win);
	byte *buff = (byte*)malloc(sizeof(byte)*nproc);
	buff[rank] = 1;
  MPI_Put(&(buff[rank]), 1, MPI_BYTE, proc, rank, 1, MPI_BYTE, win);
  /* Get data to the left of rank */
  if (rank > 0) {
      MPI_Get(buff, rank, MPI_BYTE, proc, 0, rank, MPI_BYTE, win);
  }
  /* Get data to the right of rank */
  if (rank < nproc - 1) {
      MPI_Get(&(buff[rank+1]), nproc-1-rank, MPI_BYTE, proc, rank+1, nproc-1-rank,
              MPI_BYTE, win);
  }
  MPI_Win_unlock(proc, win);
	/* check if anyone has the lock*/
  for (int i = already_locked = 0; i < nproc; i++)
    if (buff[i] && i != rank)
        already_locked = 1;

  /* Wait for notification */
  if (already_locked) {
      MPI_Status status;
      //std::cout << "waiting for notification [proc = "<<proc<<"]" << std::endl;
      MPI_Recv(NULL, 0, MPI_BYTE, MPI_ANY_SOURCE, MPI_MUTEX_TAG+id, comm, &status);
  }
	
	//std::cout << "lock acquired [proc = "<<proc<<"]" << std::endl;
  free(buff);
};


void MPIMutex::unlock(int proc) {

  int rank, nproc;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &nproc);

	byte *buff = (byte*)malloc(nproc*sizeof(byte));
	buff[rank] = 0;

  /* Get all data from the lock_buf, except the byte belonging to
   * me. Set the byte belonging to me to 0. */
  MPI_Win_lock(MPI_LOCK_EXCLUSIVE, proc, 0, win);
  MPI_Put(&(buff[rank]), 1, MPI_BYTE, proc, rank, 1, MPI_BYTE, win);
  /* Get data to the left of rank */
  if (rank > 0) {
      MPI_Get(buff, rank, MPI_BYTE, proc, 0, rank, MPI_BYTE, win);
  }
  /* Get data to the right of rank */
  if (rank < nproc - 1) {
      MPI_Get(&buff[rank+1], nproc-1-rank, MPI_BYTE, proc, rank+1, nproc-1-rank,
              MPI_BYTE, win);
  }
  MPI_Win_unlock(proc, win);

  /* Notify the next waiting process, starting to my right for fairness */
  for (int i = 1; i < nproc; i++) {
      int p = (rank + i) % nproc;
      if (buff[p] == 1) {
          //std::cout << "notifying "<<p<<"[proc = "<<proc<<"]" << std::endl;
          MPI_Send(NULL, 0, MPI_BYTE, p, MPI_MUTEX_TAG+id, comm);
          break;
      }
  }

 	//std::cout << "lock released [proc = "<<proc<<"]" << std::endl;
  free(buff);
};

