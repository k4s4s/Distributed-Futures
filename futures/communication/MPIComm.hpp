
#ifndef _MPICOMM_H
#define _MPICOMM_H

#include <mpi.h>
#include <boost/mpi/datatype.hpp>
#include <mpi_details.hpp>

namespace futures {
namespace communication {

class Shared_Address_space {
private:
	MPI_Win shared_memory_win;
	void *shared_memory;
public:
	Shared_Address_space(std::size_t size);
	~Shared_Address_space();
	template<typename T>
	int put(T& data, int dst_id, int count, int offset);

	template<typename T>
	T get(int src_id, int count, int offset);
};

class CommInterface {
public:
    CommInterface(int &argc, char**& argv);
    ~CommInterface();
    static CommInterface* create(int &argc, char**& argv);
		mutex* new_lock();
		Shared_Address_space* new_shared_space(std::size_t size);
    void send(int dst_id, int tag, int count, MPI_Datatype datatype, void* data);
    void send(int dst_id, int tag, boost::mpi::packed_oarchive& ar);
    void recv(int src_id, int tag, int count, MPI_Datatype datatype, void* data);
    void recv(int src_id, int tag, boost::mpi::packed_iarchive& ar);
    int get_procId();
    int size();
};


template<typename T>
int Shared_Address_space::put(T& data, int dst_id, int count, int offset) {
	return mpi_details::_put<T>()(data, dst_id, count, offset, shared_memory_win,
																boost::mpi::is_mpi_datatype<T>());
};

template<typename T>
T Shared_Address_space::get(int src_id, int count, int offset) {
	return mpi_details::_get<T>()(src_id, count, offset, shared_memory_win,
																boost::mpi::is_mpi_datatype<T>());
};

}//end of namespace communication
}//end of namespace futures

#endif

