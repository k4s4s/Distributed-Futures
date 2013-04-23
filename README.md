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

Other cmake flags:
-Dstats=ON/OFF		Enables/Disables runtime statistic information report
-Ddebug=ON/OFF		Enables/Disables runtime debug prints

