#ifndef __CServerTCPSocket_h__
#define __CServerTCPSocket_h__

#include "CFDCtl.h"
#include <string>

class CServerTCPSocket {
public:
	CServerTCPSocket(std::string host, int port);
	CServerTCPSocket(int port); // all addresses
	~CServerTCPSocket();

	int get_fd() { return listen_fd; }

	CFDCtl *ReceiveConnectionDirect(void);
	
private:
	void initialize(std::string host, int port);
	
	int port;
	std::string host;

	int listen_fd;
};

#endif /* __CServerTCPSocket_h__ */

