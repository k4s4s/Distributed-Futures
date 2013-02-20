

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Define a libtool archive target "libexample-@EXAMPLE_API_VERSION@.la", with
## @EXAMPLE_API_VERSION@ substituted into the generated Makefile at configure
## time.
## The libtool archive file (.la) will be installed into the directory named
## by the predefined variable $(bindir), along with the actual shared library
## file (.so).
lib_LTLIBRARIES = libfutures-@FUTURES_API_VERSION@.la

## Define the source file list for the "libexample-@EXAMPLE_API_VERSION@.la"
## target.  Note that @EXAMPLE_API_VERSION@ is not interpreted by Automake and
## will therefore be treated as if it were literally part of the target name,
## and the variable name derived from that.
## The file extension .cc is recognized by Automake, and makes it produce
## rules which invoke the C++ compiler to produce a libtool object file (.lo)
## from each source file.  Note that it is not necessary to list header files
## which are already listed elsewhere in a _HEADERS variable assignment.
libfutures_@FUTURES_API_VERSION@_la_SOURCES = communication/MPIComm.cpp \
	communication/commManager.cpp \
	scheduler/taskStack.cpp \
	scheduler/master.cpp \
	scheduler/worker.cpp \
	scheduler/RR.cpp \
	scheduler/schedManager.cpp \
	mem/SharedMemoryManager.cpp \
	stats/stats.cpp \
	futures_environment.cpp

## Instruct libtool to include ABI version information in the generated shared
## library file (.so).  The library ABI version is defined in configure.ac, so
## that all version information is kept in one place.
libfutures_@FUTURES_API_VERSION@_la_LDFLAGS = -version-info $(FUTURES_SO_VERSION)

## Define the list of public header files and their install location.  The
## nobase_ prefix instructs Automake to not strip the directory part from each
## filename, in order to avoid the need to define separate file lists for each
## installation directory.  This only works if the directory hierarchy in the
## source tree matches the hierarchy at the install location, however.
futures_includedir = $(includedir)/futures-$(FUTURES_API_VERSION)
nobase_futures_include_HEADERS = future_fwd.hpp \
	communication/communication.hpp \
	communication/commManager.hpp \
	communication/MPIComm.hpp \
	mem/SharedMemoryManager.hpp \
	scheduler/process.hpp \
	scheduler/scheduler.hpp \
	scheduler/RR.hpp \
	scheduler/taskStack.hpp \
	scheduler/schedManager.hpp \
	stats/stats.hpp \
	apply_function.hpp \
	details.hpp \
	common.hpp \
	futures_environment.hpp \
	future.hpp \
	futures.hpp

libfutures_@FUTURES_API_VERSION@_la_CPPFLAGS = -I$(top_srcdir)/mutex
libfutures_@FUTURES_API_VERSION@_la_LIBADD = -lboost_mpi -lboost_serialization

## Install the generated pkg-config file (.pc) into the expected location for
## architecture-dependent package configuration information.  Occasionally,
## pkg-config files are also used for architecture-independent data packages,
## in which case the correct install location would be $(datadir)/pkgconfig.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = futures-$(FUTURES_API_VERSION).pc

