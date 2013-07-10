Distributed-Futures
===================

Implementation of the future shared memory synchronization model for distributed memory 

Requirements boost::serialization and boost::mpi libraries

===================
Installation instructions:

cmake .
make
make install

If you need to change the default install path, run cmake with -DCMAKE_INSTALL_PREFIX:PATH=/path/to/install

If you have not installed boost in the default location, you need to define BOOST_ROOT to the root 
directory of your boost installation and add BOOST_ROOT/lib to LD_LIBRARY_PATH variable

Other cmake flags:
-Dstats=ON/OFF		Enables/Disables runtime statistic information report
-Ddebug=ON/OFF		Enables/Disables runtime debug prints

NOTE:Benchmark applications maybe need extra dependencies met.  See their readme before compiling

