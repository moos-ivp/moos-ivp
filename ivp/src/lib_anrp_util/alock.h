#ifndef __alock_h__
#define __alock_h__

#ifdef NOT_MOOS

#include <pthread.h>
#include "ssp.h"
#include <stdexcept>

class ScopedALock;

class ALock {
public:
	ALock() { 
		pthread_mutex_init(&mux, NULL);
		pthread_mutex_init(&imux, NULL);
		locked = false;
	}

	~ALock() {
		ILock();
		if(locked) Unlock();
		
		pthread_mutex_destroy(&mux);
		
		IUnlock();
		pthread_mutex_destroy(&imux);
		// won't work properly if there are multiply-threaded calls to destructor
	}

	void Unlock() {
		ILock();
		if(locked) {
			pthread_mutex_unlock(&mux);
			locked = false;
		} else {
			IUnlock();
			throw std::logic_error("Tried to unlock an already-unlocked ALock");
		}

		IUnlock();
	}

	void Lock() {
		ILock();
		if(!locked) {
			pthread_mutex_lock(&mux);
			locked = true;
		} else {
			IUnlock();
			throw std::logic_error("Tried to lock an already-locked ALock");
		}

		IUnlock();
	}

private:
	pthread_mutex_t imux, mux;

	void ILock() { pthread_mutex_lock(&imux); }
	void IUnlock() { pthread_mutex_unlock(&imux); }

	bool locked;
};

#else

#include "MOOSLock.h"

class ALock {
public:
	void Unlock() { ml.UnLock(); }
	void Lock() { ml.Lock(); }

private:
	CMOOSLock ml;
};

#endif

class ScopedALock {
public:
	ScopedALock(ALock &al) : mal(al) { mal.Lock(); }
	~ScopedALock() { mal.Unlock(); }

private:
	ALock &mal;
};

#endif
