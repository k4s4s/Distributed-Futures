
#ifndef Future_FWD_H
#define Future_FWD_H

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/mpi.hpp>

namespace futures {

class _stub {
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
    virtual ~_stub() {};
    virtual void run(int future_owner) = 0;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(_stub);

class _stub_wrapper {
private:
    friend class boost::serialization::access;
    _stub *task;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
        ar & task;
    };
public:
    _stub_wrapper() {};
    _stub_wrapper(_stub *_task) : task(_task) {};
    ~_stub_wrapper() {};
    _stub *get_task() {
        return task;
    };
};

}

#endif
