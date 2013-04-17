
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

//TODO: Find an alternative way ot implement this, so that we are indepedent from boost::mpi
template<typename TX>
struct is_primitive_type
        : public boost::mpi::is_mpi_datatype<TX>
    {};

template<typename TX>
struct is_primitive_type<TX*>
        : public boost::mpi::is_mpi_datatype<TX>
    {};

/** 
*	Template helper functions to find out is type is a container type. 
*	It works only for containers that follow stl containter type conventions
*/			
template<typename T>
struct has_const_iterator
{
private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
};

template <typename T>
struct has_begin_end
{
    template<typename C> static char (&f(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&f(...))[2];

    template<typename C> static char (&g(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&g(...))[2];

    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
};

template<typename T> 
  struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value> 
  { };

}//end of namespace details
}//end of namespace futures

#endif

