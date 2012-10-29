
CC=mpic++
CFLAGS=-g -DDEBUG
LDFLAGS=-lboost_serialization -lboost_mpi
MPI_IFLAGS=-I./mpi_futures/
ARMCI_IFLAGS=-I/home/kasas/libs/armci/include -I./armci_futures
ARMCI_LDFLAGS=-L/home/kasas/libs/armci/lib/ -larmci
TESTS_DIR=tests
BIN_DIR=bin

all: mpi_tests armci_tests

mpi_tests: $(BIN_DIR)/mpi_test1

armci_tests: $(BIN_DIR)/armci_test1

$(BIN_DIR)/mpi_test1: $(TESTS_DIR)/test1.cpp
	$(CC) $(CFLAGS) -o $(BIN_DIR)/mpi_test1 $(MPI_IFLAGS) $(TESTS_DIR)/test1.cpp $(LDFLAGS)

$(BIN_DIR)/armci_test1: $(TESTS_DIR)/test1.cpp
	$(CC) $(CFLAGS) -o $(BIN_DIR)/armci_test1 $(ARMCI_IFLAGS) -DARMCI_V $(TESTS_DIR)/test1.cpp $(LDFLAGS) $(ARMCI_LDFLAGS)

clean:
	rm -rf $(BIN_DIR)/*

