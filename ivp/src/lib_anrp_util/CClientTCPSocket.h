#ifndef __CClientTCPSocket_h__
#define __CClientTCPSocket_h__

#include "CFDCtl.h"

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

class CClientTCPSocket : public CFDCtl
{

	protected:
		int port;
		std::string hostname;

	public:
		CClientTCPSocket(std::string hostname, int port);
		~CClientTCPSocket();
};

#endif /* __CClientTCPSocket_h__ */
