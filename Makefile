
CC=mpic++
CFLAGS=-g
LDFLAGS=-lboost_serialization -lboost_mpi
ARMCI_IFLAGS=-I/home/kasas/libs/armci/include
ARMCI_LDFLAGS=-L/home/kasas/libs/armci/lib/ -larmci

all: mpi_futures

mpi_test1: test1.cpp
	$(CC) $(CFLAGS) -o mpi_test1 test1.cpp $(LDFLAGS)

armci_test1: test1.cpp
	$(CC) $(CFLAGS) -o armci_test1 $(ARMCI_IFLAGS) -DARMCI_V test1.cpp $(LDFLAGS) $(ARMCI_LDFLAGS)

clean:
	rm -rf mpi_test1 armci_test1
