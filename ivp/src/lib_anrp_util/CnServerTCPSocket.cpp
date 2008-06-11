#include "CnServerTCPSocket.h"

#include <iterator>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "ResolverLock.h"

#include <string.h>
#include <errno.h>
#include <stdexcept>
#include "ssp.h"

using namespace std;

CnServerTCPSocket::CnServerTCPSocket(int port)
{
	initialize("0.0.0.0", port);
}

CnServerTCPSocket::CnServerTCPSocket(string host, int port)
{
	initialize(host, port);
}

void CnServerTCPSocket::initialize(string host, int port)
{
	this->port = port;
	this->host = host;
	int yes = 1;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1) throw runtime_error(ssp("error with socket: %s", strerror(errno)));

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	struct sockaddr_in madr;

	LockResolver();
	struct hostent *he = gethostbyname(host.c_str());

	madr.sin_family = AF_INET;
	madr.sin_port = htons(port);
	madr.sin_addr.s_addr = *((in_addr_t *)he->h_addr);
	UnlockResolver();
	memset(madr.sin_zero, 0, 8);

	if(bind(listen_fd, (struct sockaddr *)&madr, sizeof(struct sockaddr)) == -1) {
		throw runtime_error(ssp("error binding: %s", strerror(errno)));
	}
	
	if(listen(listen_fd, 5) == -1) {
		throw runtime_error(ssp("error listening: %s", strerror(errno)));
	}
}

CnServerTCPSocket::~CnServerTCPSocket()
{
	shutdown(listen_fd, SHUT_RDWR);
}

CnFDCtl *CnServerTCPSocket::ReceiveConnectionDirect()
{
	int newfd;
	struct sockaddr_in tadr;
	int addrlen = sizeof(tadr);

	newfd = accept(listen_fd, (struct sockaddr *)&tadr, (socklen_t *)&addrlen);

	if(newfd == -1) {
		throw runtime_error(ssp("error accepting: %s", strerror(errno)));
	}

	return new CnFDCtl(newfd);
}
