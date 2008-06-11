#ifndef __CnUDPSocket_h__
#define __CnUDPSocket_h__

#include "CnDatagramCtl.h"

#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

class CnUDPSocket : public CnDatagramCtl
{
protected:
	int port;
	std::string hostname;
	struct sockaddr_in their_addr;

public:
	CnUDPSocket(std::string hostname, int port);
	~CnUDPSocket();

	struct sockaddr_in other() { return their_addr; }
};

class CnUDPSocketWrOnly : public CnDatagramCtl
{
protected:

public:
	CnUDPSocketWrOnly();
	~CnUDPSocketWrOnly();
};

#endif /* __CnUDPSocket_h__ */
