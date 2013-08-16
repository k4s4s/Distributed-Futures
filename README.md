Distributed-Futures
===================

Implementation of the future shared memory synchronization model for distributed memory 

Requirements an MPI implementation, boost::serialization and boost::mpi libraries

===================
Installation instructions:

cmake .
make
make install

If you need to change the default install path, run cmake with -DCMAKE_INSTALL_PREFIX:PATH=/path/to/install

If you need to specify the location of the MPI library, or select a different one than the one the cmake
script finds, set the cache variable MPI_COMPILER to the location of your MPI's compiler wrapper.  If the
script fails to find the MPI installation you can manually set the MPI_LIBRARY and MPI_INCLUDE_PATH cache
variables.

NOTE:Our library does not work with openmpi-1.6.5, you may need to use an older version (tested and works
with openmpi-1.4.5) 

If you have not installed boost in the default location, you need to define BOOST_ROOT to the root 
directory of your boost installation and add BOOST_ROOT/lib to LD_LIBRARY_PATH variable

Other cmake flags:
-Dstats=ON/OFF		Enables/Disables runtime statistic information report
-Ddebug=ON/OFF		Enables/Disables runtime debug prints

NOTE:Benchmark applications maybe need extra dependencies met.  See their readme before compiling

