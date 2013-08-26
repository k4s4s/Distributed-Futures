
#ifndef Future_FWD_H
#define Future_FWD_H

#include "cereal/cereal.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/polymorphic.hpp"

namespace futures {

class _stub {
public:
    virtual ~_stub() {};
    virtual void run() = 0;
    template<class Archive>
    void serialize(Archive & ar) {};
};

}

#endif
