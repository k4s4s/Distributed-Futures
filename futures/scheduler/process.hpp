#ifndef _PROCESS_H
#define _PROCESS_H

namespace futures {
namespace scheduler {

enum ProcStatus { IDLE=0, RUNNING=1, TERMINATED=2 };

class Process {
public:
	virtual ~Process() {};
	virtual void set_status(ProcStatus status) = 0;
	virtual bool terminate() = 0;
};

}
}

#endif
