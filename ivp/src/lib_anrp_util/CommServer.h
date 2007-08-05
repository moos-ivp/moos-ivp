#ifndef __CommServer_h__
#define __CommServer_h__

#include "CFDCtl.h"
#include <pthread.h>
#include <map>

using namespace std;

/* one way write only comm server */

class CommServer {
public:
	CommServer(int port);
	~CommServer();

	void WriteString(char *str, int len = -1);
	
private:
	
	static void *ThreadTrampoline(void *p);
	void AsyncThread(void);
	pthread_t thr;
	
	pthread_mutex_t client_list;
	map<int,CFDCtl *> clients;
	int cp[2];

	int port;
};

#endif /* __CommServer_h__ */

