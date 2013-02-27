
#ifndef _DETAILS_H
#define _DETAILS_H

#include <common.hpp>
#include <boost/mpi/datatype.hpp>

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
struct is_primitive_type
        : public boost::mpi::is_mpi_datatype<TX>
    {};

template<typename TX>
struct is_primitive_type<TX*>
        : public boost::mpi::is_mpi_datatype<TX>
    {};

}//end of namespace details
}//end of namespace futures

#endif

