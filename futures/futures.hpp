
#ifndef _FUTURES_H
#define _FUTURES_H

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include "future_fwd.hpp"
#include "futures_environment.hpp"
#include "future.hpp"
//#include "promise.hpp"
#include <boost/type_traits/function_traits.hpp>
#include "stats/stats.hpp"

#define IDENTITY_TYPE(parenthesized_type) \
    /* must NOT prefix this with `::` to work with parenthesized syntax */ \
    boost::function_traits< void parenthesized_type >::arg1_type
		

#define FUTURES_EXPORT_FUNCTOR(F) BOOST_CLASS_EXPORT(IDENTITY_TYPE(F))
//#define FUTURES_EXPORT_FUNCTION(F) BOOST_CLASS_EXPORT()


		//BOOST_CLASS_EXPORT(_async_stub<TYPE_NAME>)
//stupid fix...
//if serialization is not used by user program, compiler
//does not link some boost::serialization and mpl routines needed
class fix {
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
    fix() {};
    ~fix() {};
};

BOOST_CLASS_EXPORT(fix);

#endif

