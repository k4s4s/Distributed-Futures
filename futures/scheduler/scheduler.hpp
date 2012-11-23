#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "../communication/communication.hpp"

namespace futures {
namespace scheduler {

class Scheduler {
public:
	virtual ~Scheduler() {};
	virtual int nextAvaibleWorkerId() = 0;
};

}
}
#endif
