
#ifndef _FUTURES_H
#define _FUTURES_H

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include "future_fwd.hpp"
#include "futures_enviroment.hpp"
#include "future.hpp"
#include "promise.hpp"

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

