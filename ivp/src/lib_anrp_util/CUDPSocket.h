#ifndef __CUDPSocket_h__
#define __CUDPSocket_h__

#include "CDatagramCtl.h"

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

class CUDPSocket : public CDatagramCtl
{

	protected:
		int port;
		std::string hostname;
		struct sockaddr_in their_addr;

	public:
		CUDPSocket(std::string hostname, int port);
		~CUDPSocket();

		struct sockaddr_in other() { return their_addr; }
};

#endif /* __CUDPSocket_h__ */
