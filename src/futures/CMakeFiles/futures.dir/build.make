# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/kasas/work/lri/Distributed-Futures

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kasas/work/lri/Distributed-Futures

# Include any dependencies generated for this target.
include src/futures/CMakeFiles/futures.dir/depend.make

# Include the progress variables for this target.
include src/futures/CMakeFiles/futures.dir/progress.make

# Include the compile flags for this target's objects.
include src/futures/CMakeFiles/futures.dir/flags.make

src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o: src/futures/futures_environment.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/futures_environment.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/futures_environment.cpp

src/futures/CMakeFiles/futures.dir/futures_environment.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/futures_environment.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/futures_environment.cpp > CMakeFiles/futures.dir/futures_environment.cpp.i

src/futures/CMakeFiles/futures.dir/futures_environment.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/futures_environment.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/futures_environment.cpp -o CMakeFiles/futures.dir/futures_environment.cpp.s

src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.requires

src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.provides: src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.provides

src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o

src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o: src/futures/communication/commManager.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/communication/commManager.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/communication/commManager.cpp

src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/communication/commManager.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/communication/commManager.cpp > CMakeFiles/futures.dir/communication/commManager.cpp.i

src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/communication/commManager.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/communication/commManager.cpp -o CMakeFiles/futures.dir/communication/commManager.cpp.s

src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.requires

src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.provides: src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.provides

src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o

src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o: src/futures/communication/MPIComm.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/communication/MPIComm.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/communication/MPIComm.cpp

src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/communication/MPIComm.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/communication/MPIComm.cpp > CMakeFiles/futures.dir/communication/MPIComm.cpp.i

src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/communication/MPIComm.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/communication/MPIComm.cpp -o CMakeFiles/futures.dir/communication/MPIComm.cpp.s

src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.requires

src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.provides: src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.provides

src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o

src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o: src/futures/mem/SharedMemoryManager.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/mem/SharedMemoryManager.cpp

src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/mem/SharedMemoryManager.cpp > CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.i

src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/mem/SharedMemoryManager.cpp -o CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.s

src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.requires

src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.provides: src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.provides

src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o

src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o: src/futures/scheduler/schedManager.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/scheduler/schedManager.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/schedManager.cpp

src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/scheduler/schedManager.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/schedManager.cpp > CMakeFiles/futures.dir/scheduler/schedManager.cpp.i

src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/scheduler/schedManager.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/schedManager.cpp -o CMakeFiles/futures.dir/scheduler/schedManager.cpp.s

src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.requires

src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.provides: src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.provides

src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o

src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o: src/futures/scheduler/master.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/scheduler/master.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/master.cpp

src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/scheduler/master.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/master.cpp > CMakeFiles/futures.dir/scheduler/master.cpp.i

src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/scheduler/master.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/master.cpp -o CMakeFiles/futures.dir/scheduler/master.cpp.s

src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.requires

src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.provides: src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.provides

src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o

src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o: src/futures/scheduler/worker.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/scheduler/worker.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/worker.cpp

src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/scheduler/worker.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/worker.cpp > CMakeFiles/futures.dir/scheduler/worker.cpp.i

src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/scheduler/worker.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/worker.cpp -o CMakeFiles/futures.dir/scheduler/worker.cpp.s

src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.requires

src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.provides: src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.provides

src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o

src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o: src/futures/scheduler/RR.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/scheduler/RR.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/RR.cpp

src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/scheduler/RR.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/RR.cpp > CMakeFiles/futures.dir/scheduler/RR.cpp.i

src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/scheduler/RR.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/RR.cpp -o CMakeFiles/futures.dir/scheduler/RR.cpp.s

src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.requires

src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.provides: src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.provides

src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o

src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o: src/futures/scheduler/taskStack.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/scheduler/taskStack.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/taskStack.cpp

src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/scheduler/taskStack.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/taskStack.cpp > CMakeFiles/futures.dir/scheduler/taskStack.cpp.i

src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/scheduler/taskStack.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/scheduler/taskStack.cpp -o CMakeFiles/futures.dir/scheduler/taskStack.cpp.s

src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.requires

src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.provides: src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.provides

src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o

src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o: src/futures/CMakeFiles/futures.dir/flags.make
src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o: src/futures/stats/stats.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/kasas/work/lri/Distributed-Futures/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/futures.dir/stats/stats.cpp.o -c /home/kasas/work/lri/Distributed-Futures/src/futures/stats/stats.cpp

src/futures/CMakeFiles/futures.dir/stats/stats.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/futures.dir/stats/stats.cpp.i"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/kasas/work/lri/Distributed-Futures/src/futures/stats/stats.cpp > CMakeFiles/futures.dir/stats/stats.cpp.i

src/futures/CMakeFiles/futures.dir/stats/stats.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/futures.dir/stats/stats.cpp.s"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/kasas/work/lri/Distributed-Futures/src/futures/stats/stats.cpp -o CMakeFiles/futures.dir/stats/stats.cpp.s

src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.requires:
.PHONY : src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.requires

src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.provides: src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.requires
	$(MAKE) -f src/futures/CMakeFiles/futures.dir/build.make src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.provides.build
.PHONY : src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.provides

src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.provides.build: src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o

# Object files for target futures
futures_OBJECTS = \
"CMakeFiles/futures.dir/futures_environment.cpp.o" \
"CMakeFiles/futures.dir/communication/commManager.cpp.o" \
"CMakeFiles/futures.dir/communication/MPIComm.cpp.o" \
"CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o" \
"CMakeFiles/futures.dir/scheduler/schedManager.cpp.o" \
"CMakeFiles/futures.dir/scheduler/master.cpp.o" \
"CMakeFiles/futures.dir/scheduler/worker.cpp.o" \
"CMakeFiles/futures.dir/scheduler/RR.cpp.o" \
"CMakeFiles/futures.dir/scheduler/taskStack.cpp.o" \
"CMakeFiles/futures.dir/stats/stats.cpp.o"

# External object files for target futures
futures_EXTERNAL_OBJECTS =

src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/build.make
src/futures/libfutures.a: src/futures/CMakeFiles/futures.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libfutures.a"
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && $(CMAKE_COMMAND) -P CMakeFiles/futures.dir/cmake_clean_target.cmake
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/futures.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/futures/CMakeFiles/futures.dir/build: src/futures/libfutures.a
.PHONY : src/futures/CMakeFiles/futures.dir/build

src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/futures_environment.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/communication/commManager.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/communication/MPIComm.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/mem/SharedMemoryManager.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/scheduler/schedManager.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/scheduler/master.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/scheduler/worker.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/scheduler/RR.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/scheduler/taskStack.cpp.o.requires
src/futures/CMakeFiles/futures.dir/requires: src/futures/CMakeFiles/futures.dir/stats/stats.cpp.o.requires
.PHONY : src/futures/CMakeFiles/futures.dir/requires

src/futures/CMakeFiles/futures.dir/clean:
	cd /home/kasas/work/lri/Distributed-Futures/src/futures && $(CMAKE_COMMAND) -P CMakeFiles/futures.dir/cmake_clean.cmake
.PHONY : src/futures/CMakeFiles/futures.dir/clean

src/futures/CMakeFiles/futures.dir/depend:
	cd /home/kasas/work/lri/Distributed-Futures && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kasas/work/lri/Distributed-Futures /home/kasas/work/lri/Distributed-Futures/src/futures /home/kasas/work/lri/Distributed-Futures /home/kasas/work/lri/Distributed-Futures/src/futures /home/kasas/work/lri/Distributed-Futures/src/futures/CMakeFiles/futures.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/futures/CMakeFiles/futures.dir/depend

