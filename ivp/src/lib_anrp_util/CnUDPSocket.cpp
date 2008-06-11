#include "CnDatagramCtl.h"
#include "CnUDPSocket.h"

#include <stdio.h>
#include "ResolverLock.h"

#include <stdexcept>
#include <string.h>
#include <errno.h>
#include "ssp.h"

using namespace std;

CnUDPSocket::CnUDPSocket(string hostname, int port)
{
	this->port = port;
	this->hostname = hostname;
	LockResolver();
	struct hostent *he = gethostbyname(hostname.c_str());
	struct sockaddr_in any_addr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	int yes = 1;

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	UnlockResolver();
	any_addr.sin_family = AF_INET;
	any_addr.sin_port = htons(port);
	any_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(any_addr.sin_zero), 0, 8);
	memset(&(their_addr.sin_zero), 0, 8);

	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int));
	if(bind(fd, (struct sockaddr *)&any_addr, sizeof(struct sockaddr)) == -1) {
		throw runtime_error(ssp("CnUDPSocket: cannot bind: %s", strerror(errno)));
	}
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int));
	
	is_open = true;
	SetNonBlockingMode();
}

CnUDPSocket::~CnUDPSocket()
{
}
