
bin_PROGRAMS = hello power fib quicksort ping_pong benchmark1

AM_CPPFLAGS = -I$(top_srcdir)/mutex -I$(top_srcdir)/futures
AM_LDFLAGS = -L$(top_srcdir)/mutex/.libs -L$(top_srcdir)/futures/.libs \	
	-lboost_mpi -lboost_serialization \
	-lfutures-@FUTURES_API_VERSION@ -lmutex-@MUTEX_API_VERSION@

hello_SOURCES = hello.cpp

power_SOURCES = power.cpp

fib_SOURCES = fib.cpp

quicksort_SOURCES = quicksort.cpp

ping_pong_SOURCES = ping_pong.cpp

benchmark1_SOURCES = benchmark1.cpp
