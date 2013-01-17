
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
struct _get_data {
    TX operator()(communication::Shared_data* Shared_data, boost::mpl::true_) {
        TX value;
        Shared_data->get_data(&value);
        return value;
    };
    TX operator()(communication::Shared_data* Shared_data, boost::mpl::false_) {
        TX value;
        boost::mpi::packed_iarchive ia(Shared_data->get_comm());
        Shared_data->get_data(ia);
        // Deserialize the data in the message
        ia >> value;
        return value;
    };
};

template<typename TX>
struct _get_data<TX*> {
    TX* operator()(communication::Shared_data* Shared_data, boost::mpl::true_) {
        TX* value = new TX[Shared_data->get_dataSize()];
        Shared_data->get_data(value);
        return value;
    };
    TX* operator()(communication::Shared_data* Shared_data, boost::mpl::false_) {
        TX* values = new TX[Shared_data->get_dataSize()];
        boost::mpi::packed_iarchive ia(Shared_data->get_comm());
        Shared_data->get_data(ia);
        // Determine how much data we are going to receive
        int count;
        ia >> count;
        // Deserialize the data in the message
        boost::serialization::array<TX> arr(values, count);
        ia >> arr;
        return values;
    };
};

template<typename TX>
struct _set_data {
    void operator()(communication::Shared_data* Shared_data,
                    TX value, boost::mpl::true_) {
        Shared_data->set_data(&value);
    };
    void operator()(communication::Shared_data* Shared_data,
                    TX value, boost::mpl::false_) {
        boost::mpi::packed_oarchive oa(Shared_data->get_comm());
        oa << value;
        Shared_data->set_data(oa);
    };
};

template<typename TX>
struct _set_data<TX*> {
    void operator()(communication::Shared_data* Shared_data,
                    TX* value, boost::mpl::true_) {
        Shared_data->set_data(value);
    };
    void operator()(communication::Shared_data* Shared_data,
                    TX* values, boost::mpl::false_) {
        int n = Shared_data->get_dataSize();
        boost::mpi::packed_oarchive oa(Shared_data->get_comm());
        oa << n << boost::serialization::make_array(values, n);
        Shared_data->set_data(oa);
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
struct _is_mpi_datatype
        : public boost::mpi::is_mpi_datatype<TX>
    {};

template<typename TX>
struct _is_mpi_datatype<TX*>
        : public boost::mpi::is_mpi_datatype<TX>
    {};

}//end of namespace details
}//end of namespace futures

#endif

