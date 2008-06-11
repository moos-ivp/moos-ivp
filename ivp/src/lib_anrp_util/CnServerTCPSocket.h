#ifndef __CnServerTCPSocket_h__
#define __CnServerTCPSocket_h__

#include "CnFDCtl.h"
#include <string>

class CnServerTCPSocket {
public:
	CnServerTCPSocket(std::string host, int port);
	CnServerTCPSocket(int port); // all addresses
	~CnServerTCPSocket();

	int get_fd() { return listen_fd; }

	CnFDCtl *ReceiveConnectionDirect(void);
	
private:
	void initialize(std::string host, int port);
	
	int port;
	std::string host;

	int listen_fd;
};

#endif /* __CnServerTCPSocket_h__ */

