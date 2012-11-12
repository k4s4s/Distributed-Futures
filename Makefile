
CC=mpic++
CFLAGS=-gstabs+ -DDEBUG -std=c++0x
LDFLAGS=-lboost_serialization -L/home/anthi/libs/armci/lib/ -larmci
MPI_IFLAGS=-I./futures/ -I./futures/communication/ -I/home/anthi/libs/armci/include -I./armci_futures
TESTS_DIR=tests
BIN_DIR=bin

core = $(wildcard futures/*.hpp)
communication = $(wildcard futures/communication/*.hpp)
armci_includes = $(wildcard armci_futures/*.hpp)

.PHONY: directories

all: directories mpi_tests

directories: $(BIN_DIR)

$(BIN_DIR): 
	@mkdir -p $(BIN_DIR)

mpi_tests: $(BIN_DIR) $(BIN_DIR)/hello $(BIN_DIR)/test_arrays.cpp

$(BIN_DIR)/hello: $(TESTS_DIR)/hello.cpp $(core) $(communication)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/hello $(MPI_IFLAGS) $(TESTS_DIR)/hello.cpp $(LDFLAGS)

$(BIN_DIR)/test_arrays.cpp: $(TESTS_DIR)/test_arrays.cpp $(core) $(communication)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/test_arrays $(MPI_IFLAGS) $(TESTS_DIR)/test_arrays.cpp $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

