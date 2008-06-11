#include "CommServer.h"

#include <iterator>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

CommServer::CommServer(int port)
{
	this->port = port;

	pipe(cp);
        thr.Initialise(&(CommServer::ThreadTrampoline), this);
}

CommServer::~CommServer()
{
}

void CommServer::WriteString(char *str, int len)
{
	client_list.Lock();
	if(len == -1) len = strlen(str);

	map<int,CFDCtl *>::iterator it;

	for(it = clients.begin(); it != clients.end(); it++) {
		((*it).second)->AppendWriteQueue(str, len);
	}

	client_list.UnLock();

	/* force a wakeup */
	write(cp[1], "W", 1);
}

void CommServer::WriteStringIfEmpty(char *str, int len)
{
	client_list.Lock();
	if(len == -1) len = strlen(str);

	map<int,CFDCtl *>::iterator it;

	for(it = clients.begin(); it != clients.end(); it++) {
		CFDCtl *p = it->second;
		if(p->WriteBufLen() == 0) {
			p->AppendWriteQueue(str, len);
		}
	}

	client_list.UnLock();

	write(cp[1], "W", 1);
}

bool CommServer::ThreadTrampoline(void *p)
{
	((CommServer *)p)->AsyncThread();
	return true;
}

void CommServer::AsyncThread()
{
	int sfd, yes = 1;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	struct sockaddr_in madr;

	madr.sin_family = AF_INET;
	madr.sin_port = htons(port);
	madr.sin_addr.s_addr = INADDR_ANY;
	memset(madr.sin_zero, 0, 8);

	bind(sfd, (struct sockaddr *)&madr, sizeof(struct sockaddr));

	listen(sfd, 3);

	bool done = false;

	while(!done) {
		// build fd list
		struct pollfd *fds;
		
		client_list.Lock();
		fds = (struct pollfd *)malloc(sizeof(struct pollfd) * 2);
		memset(fds, 0, sizeof(struct pollfd) * 2);
		
		fds[0].fd = sfd;
		fds[0].events = POLLIN;
		fds[1].fd = cp[0];
		fds[1].events = POLLIN;

		map<int,CFDCtl *>::iterator it;
		int i = 2;
		for(it = clients.begin(); it != clients.end(); it++) {
			fds = (struct pollfd *)realloc(fds,
					sizeof(struct pollfd) * (i+1));
			memset(&(fds[i]), 0, sizeof(struct pollfd));
			
			fds[i].fd = ((*it).second)->get_fd();
			fds[i].events = POLLIN | POLLERR | POLLHUP;
			
			if(((*it).second)->WriteBufSize()) {
				fds[i].events |= POLLOUT;
			}
			
			i++;
		}
		client_list.UnLock();

		int pr = poll(fds, i, -1);
		
		if(pr == -1) {
			perror("poll");
			throw;
		}

		if(fds[1].revents & POLLIN) {
			char c;
			read(fds[1].fd, &c, 1);
			switch(c) {
			case 'W': // wakeup
				break;
			}
		}

		if(fds[0].revents & POLLIN) {
			// new connection
			int newfd;
			struct sockaddr_in tadr;
			int addrlen = sizeof(tadr);
			newfd = accept(sfd, (struct sockaddr *)&tadr, (socklen_t *)&addrlen);

			printf("new connection from %s\n", inet_ntoa(tadr.sin_addr));

			client_list.Lock();
			clients[newfd] = new CFDCtl(newfd);
			client_list.UnLock();
		}

		for(int j=2; j<i; j++) {
			if(fds[j].revents & (POLLIN | POLLERR | POLLHUP)) {
				// remove... 
				printf("removing connection on fd %i\n",
						fds[j].fd);
				client_list.Lock();
				close(fds[j].fd);
				delete clients[fds[j].fd];
				clients.erase(fds[j].fd);
				client_list.UnLock();
			} else if(fds[j].revents & POLLOUT) {
				client_list.Lock();
				(clients[fds[j].fd])->NonBlockingWrite();
				client_list.UnLock();
			}
		}

		free(fds);
	}
}
