CC=mpic++
CCFLAGS=-gstabs+ -DDEBUG -std=c++0x -DARMCI_MPI_V
LDFLAGS=-lboost_serialization -lboost_mpi -L./lib/ -lfuture -lmutex -L/home/kasas/libs/armci_mpi/lib/ -larmci
INCLUDES=-I./futures/ -I./futures/communication/ -I./mutex/ -I/home/kasas/libs/armci_mpi/include
LIB_DIR=lib
BIN_DIR=bin

FUTURES_SOURCES = $(wildcard futures/*.cpp)
FUTURES_SOURCES += $(wildcard futures/communication/*.cpp)
FUTURES_HEADERS = $(wildcard futures/*.hpp)
FUTURES_HEADERS += $(wildcard futures/communication/*.hpp)
FUTURES_OBJECTS = $(FUTURES_SOURCES:.cpp=.o)
FUTURES_LIB = $(LIB_DIR)/libfuture.a

MUTEX_SOURCES = $(wildcard mutex/*.cpp)
MUTEX_HEADERS = $(wildcard mutex/*.hpp)
MUTEX_OBJECTS = $(MUTEX_SOURCES:.cpp=.o)
MUTEX_LIB = $(LIB_DIR)/libmutex.a

TESTS_SOURCES =  $(wildcard tests/*.cpp)
TESTS_OBJECTS = $(TESTS_SOURCES:.cpp=.o)
TESTS_TARGETS = $(TESTS_SOURCES:.cpp=.out)

.PHONY: directories

all: directories mutex_lib futures_lib tests

directories: $(LIB_DIR)

$(BIN_DIR): 
	@mkdir -p $(BIN_DIR)

$(LIB_DIR): 
	@mkdir -p $(LIB_DIR)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(MUTEX_LIB): $(MUTEX_OBJECTS)
	ar rcs $(MUTEX_LIB) $(MUTEX_OBJECTS)

mutex_lib: $(LIB_DIR) $(MUTEX_LIB)

$(FUTURES_LIB): $(FUTURES_OBJECTS)
	ar rcs $(FUTURES_LIB) $(FUTURES_OBJECTS)

futures_lib: $(LIB_DIR) $(FUTURES_LIB)

.cpp.out: $(TESTS_OBJECTS)
	$(CC) $(INCLUDES) $(CCFLAGS) -o $@ $< $(LDFLAGS)

tests: $(TESTS_TARGETS)

clean:
	rm -rf $(LIB_DIR) $(MUTEX_OBJECTS) $(FUTURES_OBJECTS) $(TESTS_OBJECTS) $(TESTS_TARGETS)

