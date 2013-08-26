
#ifndef _FUTURES_H
#define _FUTURES_H

/*! \file futures.hpp
 *	\brief The header file that must be included 
 *				 by any program useing the futures librarys.
*/

#include <memory>
#include "cereal/cereal.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/polymorphic.hpp"

#include "future_fwd.hpp"
#include "futures_environment.hpp"
#include "future.hpp"
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
#define FUTURES_EXPORT_FUNCTOR(F) \
  namespace cereal {                                     \
  namespace detail {                                     \
  template <>                                            \
  struct binding_name<IDENTITY_TYPE(F)>                                 \
  {                                                      \
    static constexpr char const * name() { return #F; }; \
  };                                                     \
  } } /* end namespaces */                               \
  CEREAL_BIND_TO_ARCHIVES(IDENTITY_TYPE(F));

/**
 *	MACRO used to implement the non-intrusive serialization routine
 *  needed by the boost::serialization library.  It can only be used
 *	for simple stateless functor objects that have no member variables
 *  that need to be serialized.
 *	@param C the functor object to be serialized
*/
#define FUTURES_SERIALIZE_CLASS(C) \
	template<class Archive> \
	void serialize(Archive & ar, C c) {};

#endif

