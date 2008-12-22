#ifndef __athread_h__
#define __athread_h__

#ifdef NOT_MOOS

#include <pthread.h>
#include <stdexcept>
#include "ssp.h"

class AThread {
public:
	AThread() { 
		running = false;
	}

	~AThread() {
		if(running) {
			pthread_cancel(thr);
			pthread_join(thr, NULL);
			running = false;
		}
	}

	void Run(void *(*aufn)(void *), void *aud) {
		ufn = aufn; ud = aud;
		pthread_create(&thr, NULL, &intf, this);
	}

	void *Stop() {
		void *ret;
		if(running) {
			pthread_cancel(thr);
			pthread_join(thr, &ret);
			running = false;
			return ret;
		} else {
			throw std::logic_error("Stopped a thread that wasn't running");
		}
	}

private:
	pthread_t thr;
	void *(*ufn)(void *);
	void *ud;

	static void *intf(void *arg) {
		AThread *me = ((AThread *)arg);

		me->running = true;
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

		return me->ufn(me->ud);
	}

	bool running;
};

#else

#include "MOOSThread.h"

class AThread {
public:
	AThread() {}
	~AThread() {}
	
	void Run(void *(*ufn)(void *), void *aud) { uf = ufn; ud = aud; th.Initialise(tr, this); }
	void *Stop() { th.Stop(); return NULL; }

private:
	void *(*uf)(void *);
	void *ud;

	static bool tr(void *arg) {
		AThread *me = (AThread *)arg;
			
		me->uf(me->ud);

		return true;
	}

	CMOOSThread th;
};

#endif

#endif
