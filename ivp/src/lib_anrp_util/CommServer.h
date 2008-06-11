#ifndef __CommServer_h__
#define __CommServer_h__

#include "CFDCtl.h"
#include <MOOSThread.h>
#include <MOOSLock.h>
#include <map>

using namespace std;

/* one way write only comm server */

class CommServer {
public:
	CommServer(int port);
	~CommServer();

	void WriteString(char *str, int len = -1);
	void WriteStringIfEmpty(char *str, int len = -1);
	
private:
	
        // The 'bool' return value is just to satisfy CMOOSThread's signature
        // requirements.
	static bool ThreadTrampoline(void *p);

	void AsyncThread(void);
	CMOOSThread thr;
	
	CMOOSLock client_list;
	map<int,CFDCtl *> clients;
	int cp[2];

	int port;
};

#endif /* __CommServer_h__ */

