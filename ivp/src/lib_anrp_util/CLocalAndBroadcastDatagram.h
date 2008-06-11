#ifndef __CLocalAndBroadcastDatagram_h__
#define __CLocalAndBroadcastDatagram_h__

#include <pthread.h>
#include "CnUDPSocket.h"
#include "CnClientTCPSocket.h"
#include "CnServerTCPSocket.h"
#include <string>
#include <list>
#include <errno.h>
#include <string.h>
#include "dtime.h"
#include <math.h>
#include "ssp.h"

class CLocalAndBroadcastDatagram {
public:
	CLocalAndBroadcastDatagram(std::string localbindaddr, std::string remotebindaddr, int port);
	~CLocalAndBroadcastDatagram();

	void Send(ndatagram_t &dg);
	ndatagram_t blankdg(void);

	bool Readable() { 
		bool r = false;
		inL(); if(in.size()) r = true; inU();
		return r;
	}

	ndatagram_t Read(double timeout = 1) {
		ndatagram_t dg;
		inL();
		if(timeout == 0 && in.size() == 0) {
			inU();
			throw std::logic_error("Cannot read from empty queue");
		} else if(in.size() > 0) {
			dg = in.front();
			in.pop_front();
			inU();
			return dg;
		} else {
			if(timeout < 0) { // infinite wait
				if(pthread_cond_wait(&inC, &inM) != 0) {
					throw std::runtime_error(ssp("Failed on pthread_cond_wait: %s", strerror(errno)));
				}
				dg = in.front();
				in.pop_front();
				inU();
				return dg;
			} else {
				double ct = dtime(); ct += timeout;

				struct timespec ttw;

				ttw.tv_nsec = int(timeout*1000000000) % 1000000000;
				ttw.tv_sec = (int)floor(timeout);

				if(pthread_cond_timedwait(&inC, &inM, &ttw) != 0) {
					throw std::runtime_error(ssp("Failed waiting on read/condition: %s", strerror(errno)));
				} else {
					dg = in.front();
					in.pop_front();
					inU();
					return dg;
				}
			}
		}
	}
private:
	std::list<ndatagram_t> in;
	pthread_mutex_t inM;
	pthread_cond_t inC;
	void inL() { pthread_mutex_lock(&inM); }
	void inU() { pthread_mutex_unlock(&inM); }
	pthread_t ct;
	static void *tramp(void *arg) { ((CLocalAndBroadcastDatagram *)arg)->Thread(); return NULL; }
	void Thread();
	bool thrrun;
	void rebuild_lfcs();

	CnClientTCPSocket *lfcs;

	std::string lba, rba;
	int port;

	// owned by fork'd process
	void GenController();
	void ControllerProc();
	CnServerTCPSocket *serv;
};

#endif
