
#ifndef _FUTURES_H
#define _FUTURES_H

/*! \file futures.hpp
 *	\brief The header file that must be included 
 *				 by any program useing the futures librarys.
*/

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
		
/**
 *	MACRO used to expose a functor object to the boost::serialization
 *	library.
 *	@param F the functor object passed as a template argument to 
 *				 async_function<F>.  Example:FUTURES_EXPORT_FUNCTOR(async_function<foo>)
 *  			 If a functor call needs arguments, then f should be 
 *				 async_function<foo, arg1::type, arg2::type, ..., argN::type>.  
*/
#define FUTURES_EXPORT_FUNCTOR(F) BOOST_CLASS_EXPORT(IDENTITY_TYPE(F))

/**
 *	MACRO used to implement the non-intrusive serialization routine
 *  needed by the boost::serialization library.  It can only be used
 *	for simple stateless functor objects that have no member variables
 *  that need to be serialized.
 *	@param C the functor object to be serialized
*/
#define FUTURES_SERIALIZE_CLASS(C) \
	namespace boost { \
	namespace serialization { \
	template<class Archive> \
	void serialize(Archive & ar, C c, const unsigned int version) {}; \
	} \
	}

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

