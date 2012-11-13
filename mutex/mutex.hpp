
#ifndef _MUTEX_H
#define _MUTEX_H

namespace details {
	static unsigned int mutex_count = 0;
}

class mutex {
public:
	virtual ~mutex() {};
	virtual bool try_lock(int proc) = 0;
	virtual void lock(int proc) = 0;
	virtual void unlock(int proc) = 0;
};

#endif
