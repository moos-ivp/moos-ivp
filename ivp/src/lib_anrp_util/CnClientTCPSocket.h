#ifndef __CnClientTCPSocket_h__
#define __CnClientTCPSocket_h__

#include "CnFDCtl.h"

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

class CnClientTCPSocket : public CnFDCtl
{

	protected:
		int port;
		std::string hostname;

	public:
		CnClientTCPSocket(std::string hostname, int port);
		~CnClientTCPSocket();
};

#endif /* __CnClientTCPSocket_h__ */
