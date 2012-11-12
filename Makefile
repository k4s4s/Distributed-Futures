
CC=mpic++
CFLAGS=-gstabs+ -DDEBUG -std=c++0x
LDFLAGS=-lboost_serialization -lboost_mpi
MPI_IFLAGS=-I./futures/ -I./futures/communication/
ARMCI_IFLAGS=-I/home/kasas/libs/armci/include -I./armci_futures
ARMCI_LDFLAGS=-L/home/kasas/libs/armci/lib/ -larmci
TESTS_DIR=tests
BIN_DIR=bin

core = $(wildcard futures/*.hpp)
communication = $(wildcard futures/communication/*.hpp)
armci_includes = $(wildcard armci_futures/*.hpp)

.PHONY: directories

all: directories mpi_tests armci_tests

directories: $(BIN_DIR)

$(BIN_DIR): 
	@mkdir -p $(BIN_DIR)

mpi_tests: $(BIN_DIR) $(BIN_DIR)/mpi_test1

armci_tests: $(BIN_DIR) $(BIN_DIR)/armci_test1

$(BIN_DIR)/mpi_test1: $(TESTS_DIR)/test1.cpp $(core) $(communication)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/mpi_test1 $(MPI_IFLAGS) $(TESTS_DIR)/test1.cpp $(LDFLAGS)

$(BIN_DIR)/armci_test1: $(TESTS_DIR)/test1.cpp $(armci_includes)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/armci_test1 $(ARMCI_IFLAGS) -DARMCI_V $(TESTS_DIR)/test1.cpp $(LDFLAGS) $(ARMCI_LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

