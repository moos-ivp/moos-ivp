// $Header: /raid/cvs-server/REPOSITORY/software/MOOS/apps/multi/MOOSBlink/CMOOSBlink.h,v 1.2 2007/01/31 20:13:48 anrp Exp $
// (c) 2004 
// CMOOSBlink.h: interface for the CMOOSBlink class.
////////////////////////////////////////////////

#ifndef __CMOOSBlink_h__
#define __CMOOSBlink_h__

#include "MOOSLib.h"
#include <string>
#include <queue>
#include <utility>
#include "dtime.h"
#include <pthread.h>

// source_var, pair<dest_db, dest_var>
struct config_var_t {
	config_var_t() : todb("INVALID"), tovar("INVALID"), lasttx(0), min_time(0) {}
	config_var_t(std::string db, std::string var, double min) :
		todb(db), tovar(var), lasttx(0), min_time(min) {}
	std::string todb;
	std::string tovar;
	double lasttx;
	double min_time;
};

typedef std::map<std::string,config_var_t> config_t;

class seq_list_t : public std::list<std::pair<uint32_t, double> > {
public:
	seq_list_t() {
		pthread_mutex_init(&mm, NULL);
	}

	void clear_old(double max_age) {
		double now = dtime();
		lock();
	restart:
		for(seq_list_t::iterator it = begin(); it != end(); it++) {
			if(it->second + max_age < now) {
				erase(it);
				goto restart;
			}
		}
		unlock();
	}

	bool has(uint32_t seq) {
		lock();
		for(seq_list_t::iterator it = begin(); it != end(); it++) {
			if(it->first == seq) {
				unlock();
				return true;
			}
		}
		unlock();
		return false;
	}

	void add(uint32_t seq) {
		lock();
		push_front(std::make_pair(seq, dtime()));
		unlock();
	}
private:
	pthread_mutex_t mm;
	void lock(void) { pthread_mutex_lock(&mm); }
	void unlock(void) {pthread_mutex_unlock(&mm); }
};

class CMOOSBlink : public CMOOSApp
{
public:
	CMOOSBlink();
	virtual ~CMOOSBlink();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();

protected:
	// insert local vars here

	std::string broadcastaddr;
	std::string communityname;
	config_t cfg;

	pthread_t listen_thread;
	static void *listen_thread_proc_tramp(void *arg) {
		((CMOOSBlink *)arg)->listen_thread_p();
		return NULL;
	}
	void listen_thread_p(void);
	pthread_mutex_t lm;
	void L_l(void) { pthread_mutex_lock(&lm); }
	void U_l(void) { pthread_mutex_unlock(&lm); }
	std::list<CMOOSMsg> in;

	pthread_t write_thread;
	static void *write_thread_proc_tramp(void *arg) {
		((CMOOSBlink *)arg)->write_thread_p();
		return NULL;
	}
	void write_thread_p(void);
	pthread_mutex_t wm;
	void L_w(void) { pthread_mutex_lock(&wm); }
	void U_w(void) { pthread_mutex_unlock(&wm); }
	std::list<CMOOSMsg> out;

	seq_list_t recent;
};

#endif /* __CMOOSBlink_h__ */
