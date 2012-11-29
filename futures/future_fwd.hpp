
#ifndef Future_FWD_H
#define Future_FWD_H

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/mpi.hpp>

namespace futures {

class AsyncTask {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	virtual ~AsyncTask() {};
	virtual void operator()(int id) = 0; 
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AsyncTask);

class TaskWrapper {
private:
  friend class boost::serialization::access;
	AsyncTask *task;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & task;
	};
public:
	TaskWrapper() {};
	TaskWrapper(AsyncTask *_task) : task(_task) {};
	~TaskWrapper() {};
	AsyncTask *get_task() { return task; };
};

}

#endif
