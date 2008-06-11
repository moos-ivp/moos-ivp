#include "CServerTCPSocket.h"

#include <iterator>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "ResolverLock.h"

using namespace std;

CServerTCPSocket::CServerTCPSocket(int port)
{
	initialize("0.0.0.0", port);
}

CServerTCPSocket::CServerTCPSocket(string host, int port)
{
	initialize(host, port);
}

void CServerTCPSocket::initialize(string host, int port)
{
	this->port = port;
	this->host = host;
	int yes = 1;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	struct sockaddr_in madr;

	LockResolver();
	struct hostent *he = gethostbyname(host.c_str());

	madr.sin_family = AF_INET;
	madr.sin_port = htons(port);
	madr.sin_addr.s_addr = *((in_addr_t *)he->h_addr);
	UnlockResolver();
	memset(madr.sin_zero, 0, 8);

	bind(listen_fd, (struct sockaddr *)&madr, sizeof(struct sockaddr));

	listen(listen_fd, 5);
}

CServerTCPSocket::~CServerTCPSocket()
{
	shutdown(listen_fd, SHUT_RDWR);
}

CFDCtl *CServerTCPSocket::ReceiveConnectionDirect()
{
	int newfd;
	struct sockaddr_in tadr;
	int addrlen = sizeof(tadr);

	newfd = accept(listen_fd, (struct sockaddr *)&tadr, (socklen_t *)&addrlen);

	if(newfd == -1) return NULL;

	return new CFDCtl(newfd);
}
