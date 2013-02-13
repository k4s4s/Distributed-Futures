
#ifndef _DETAILS_H
#define _DETAILS_H

#include <boost/mpi/datatype.hpp>
#include <boost/mpi/packed_oarchive.hpp>
#include <boost/mpi/packed_iarchive.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include "communication/communication.hpp"
#include "common.hpp"

namespace futures {
namespace details {

template<typename TX>
struct _sizeof {
    int operator()(boost::mpl::true_) {
        return sizeof(TX);
    };
    int operator()(boost::mpl::false_) {
        return sizeof(TX);
    };
};

template<typename TX>
struct _sizeof<TX*> {
    int operator()(boost::mpl::true_) {
        return sizeof(TX);
    };
    int operator()(boost::mpl::false_) {
        return sizeof(boost::serialization::array<TX>);
    };
};

template<typename TX>
struct _get_mpi_datatype {
    MPI_Datatype operator()(boost::mpl::true_) {
        return boost::mpi::get_mpi_datatype<TX>(TX());
    };
    MPI_Datatype operator()(boost::mpl::false_) {
        return MPI_DATATYPE_NULL;
    };
};

template<typename TX>
struct	_get_mpi_datatype<TX*> {
    MPI_Datatype operator()(boost::mpl::true_) {
        return boost::mpi::get_mpi_datatype<TX>(TX());
    };
    MPI_Datatype operator()(boost::mpl::false_) {
        return MPI_DATATYPE_NULL;
    };
};

template<typename TX>
struct is_primitive_type
        : public boost::mpi::is_mpi_datatype<TX>
    {};

template<typename TX>
struct is_primitive_type<TX*>
        : public boost::mpi::is_mpi_datatype<TX>
    {};


template<typename TX>
struct _get {
    TX operator()(communication::Shared_Address_space *shared_space,
									int dst_id,
									int size,
									int offset, 
									boost::mpl::true_) {
			TX data;
			shared_space->get(&data, dst_id, size, offset,  
				_get_mpi_datatype<TX>()(is_primitive_type<TX>()));
			return data;
		}
    TX operator()(communication::Shared_Address_space *shared_space,
									int dst_id,
									int size,
									int offset, 
									boost::mpl::false_) {
        TX data;
        boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
        shared_space->get(ia, dst_id, offset);
        // Deserialize the data in the message
        ia >> data;
        return data;
    };
};

template<typename TX>
struct _get<TX*> {
    TX* operator()(communication::Shared_Address_space *shared_space,
									int dst_id,
									int size,
									int offset, 
									boost::mpl::true_) {
        TX* data = new TX[size];
				shared_space->get(data, dst_id, size, offset,  
					_get_mpi_datatype<TX>()(is_primitive_type<TX>()));
        return data;
    };
    TX* operator()(communication::Shared_Address_space *shared_space,
									int dst_id,
									int size,
									int offset, 
									boost::mpl::false_) {
        TX* data = new TX[size];
        boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
        shared_space->get(ia, dst_id, offset);
        // Determine how much data we are going to receive
        int count;
        ia >> count;
        // Deserialize the data in the message
        boost::serialization::array<TX> arr(data, count);
        ia >> arr;
        return data; //FIXME: need to destory array object at some point
    };
};

template<typename TX>
struct _put {
    void operator()(communication::Shared_Address_space *shared_space,
										TX data,
										int dst_id,
										int size,
										int offset, 
										boost::mpl::true_) {
        shared_space->put(&data, dst_id, size,offset,
					_get_mpi_datatype<TX>()(is_primitive_type<TX>()));
    };
    void operator()(communication::Shared_Address_space *shared_space,
										TX data,
										int dst_id,
										int size,
										int offset, 
										boost::mpl::false_) {
        boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
        oa << data;
        shared_space->put(oa, dst_id, offset);
    };
};

template<typename TX>
struct _put<TX*> {
    void operator()(communication::Shared_Address_space *shared_space,
										TX* data,
										int dst_id,
										int size,
										int offset, 
										boost::mpl::true_) {
        shared_space->put(data, dst_id, size, offset,
					_get_mpi_datatype<TX>()(is_primitive_type<TX>()));
    };
    void operator()(communication::Shared_Address_space *shared_space,
										TX* data,
										int dst_id,
										int size,
										int offset, 
										boost::mpl::false_) {
        boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
        oa << size << boost::serialization::make_array(data, size);
        shared_space->put(oa, dst_id, offset);
    };
};

template<typename TX>
struct _send_data {
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX data, boost::mpl::true_) {
        commInterface->send(dst_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), &data);
    };
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX data, boost::mpl::false_) {
        boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
        oa << data;
        commInterface->send(dst_id, tag, oa);
    };
};

template<typename TX>
struct _send_data<TX*> {
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX* data, boost::mpl::true_) {
        commInterface->send(dst_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), data);
    };
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX* data, boost::mpl::false_) {
        int n = sizeof(TX); //FIXME: maybe add support for arrays, not necessary at the moment though
        boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
        oa << n << boost::serialization::make_array(data, n);
        commInterface->send(dst_id, tag, oa);
    };
};

template<typename TX>
struct _recv_data {
    TX operator()(communication::CommInterface* commInterface, int src_id, int tag,
                  boost::mpl::true_) {
        TX data;
        commInterface->recv(src_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), &data);
        return data;
    };
    TX operator()(communication::CommInterface* commInterface, int src_id, int tag,
                  boost::mpl::false_) {
        TX data;
        boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
        commInterface->recv(src_id, tag, ia);
        ia >> data;
        return data;
    };
};

template<typename TX>
struct _recv_data<TX*> {
    TX* operator()(communication::CommInterface* commInterface, int src_id, int tag,
                   boost::mpl::true_) {
        TX* data = new TX[1];
        commInterface->recv(src_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), data);
        return data;
    };
    TX* operator()(communication::CommInterface* commInterface, int src_id, int tag,
                   boost::mpl::false_) {
        TX* data = new TX[1];
        boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
        commInterface->recv(src_id, tag, ia);
        // Determine how much data we are going to receive
        int count;
        ia >> count;
        // Deserialize the data in the message
        boost::serialization::array<TX> arr(data, count);
        ia >> arr;
        return data;
    };
};

}//end of namespace details
}//end of namespace futures

#endif

