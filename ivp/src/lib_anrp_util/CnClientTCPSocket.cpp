#include "CnFDCtl.h"
#include "CnClientTCPSocket.h"

#include <stdio.h>
#include "ResolverLock.h"

#include <string.h>
#include <errno.h>
#include "ssp.h"
#include <stdexcept>

using namespace std;

CnClientTCPSocket::CnClientTCPSocket(string hostname, int port)
{
	this->port = port;
	this->hostname = hostname;
	LockResolver();
	struct hostent *he = gethostbyname(hostname.c_str());
	struct sockaddr_in their_addr;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	UnlockResolver();
	memset(&(their_addr.sin_zero), 0, 8);

	if(connect(sfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
		close(sfd);
		throw runtime_error(ssp("failure to connect: %s", strerror(errno)));
	}

	fd = sfd;
	is_open = true;
	SetNonBlockingMode();
}

CnClientTCPSocket::~CnClientTCPSocket()
{
	if (is_open) {
		Close();
	}
}

