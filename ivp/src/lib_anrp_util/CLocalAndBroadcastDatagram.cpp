#include "CLocalAndBroadcastDatagram.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <list>
#include <poll.h>

using namespace std;

CLocalAndBroadcastDatagram::CLocalAndBroadcastDatagram(string lba, string rba, int port)
{
	this->lba = lba;
	this->rba = rba;
	this->port = port;

	GenController();
	
	thrrun = false;
	lfcs = NULL;
	pthread_mutex_init(&inM, NULL);
	pthread_cond_init(&inC, NULL);
	rebuild_lfcs();
	pthread_create(&ct, NULL, &tramp, this);
}

CLocalAndBroadcastDatagram::~CLocalAndBroadcastDatagram()
{
	thrrun = false;
	pthread_join(ct, NULL);
}

void CLocalAndBroadcastDatagram::rebuild_lfcs(void)
{
	while(lfcs == NULL) {
		try {
			lfcs = new CnClientTCPSocket("127.0.0.1", port);
			printf("Connected to internal server\n");
		} catch(const exception &e) {
			lfcs = NULL;
			usleep(100000);
		}
	}
}

void CLocalAndBroadcastDatagram::Thread(void)
{
	thrrun = true;

	while(thrrun) {
		if(lfcs == NULL) {
			rebuild_lfcs();
		}

		try {
			lfcs->BlockingRead();
			while(lfcs->ReadBufLen() > sizeof(int)) {
				bom b = lfcs->Peek(sizeof(int));
				int s; memcpy(&s, b.p(), sizeof(int));

				if(lfcs->ReadBufLen() >= sizeof(int)+s) {
					lfcs->Read(sizeof(int));
					bom spkt = lfcs->Read(s);

					ndatagram_t pkt(spkt);

					inL();
					in.push_back(pkt);
					inU();
					pthread_cond_signal(&inC);
				} else break;
			}
		} catch(const exception &e) {
			delete lfcs;
			lfcs = NULL;
		}
	}
}

void CLocalAndBroadcastDatagram::Send(ndatagram_t &dg)
{
	bom pkt = dg.serialize();
	int s = pkt.size();
	
	lfcs->AppendWriteQueue((char *)&s, sizeof(int));
	lfcs->AppendWriteQueue(pkt);
	lfcs->FullQueueWrite();
}

ndatagram_t CLocalAndBroadcastDatagram::blankdg(void)
{
	ndatagram_t rv;

	return rv;
}

void CLocalAndBroadcastDatagram::GenController()
{
	printf("Generating controller process\n");
	try {
		CnServerTCPSocket *s = new CnServerTCPSocket("127.0.0.1", port);
		// didn't except -> got the listening socket
		serv = s;

		if(fork() == 0) {
			// i'm the child
			signal(SIGHUP, SIG_IGN);
			signal(SIGINT, SIG_IGN);
			//signal(SIGTERM, SIG_IGN);
			//close(0);
			//close(1);
			//close(2);

			try {
				ControllerProc();
			} catch (const exception &e) {
				printf("Controller threw exception: %s\n", e.what());
				exit(0);
			}
		} else {
			signal(SIGCHLD, SIG_IGN);
		}
	} catch(const exception &e) {
		printf("Didn't win the contest on port %i\n", port);
	}
}

void CLocalAndBroadcastDatagram::ControllerProc()
{
	CnUDPSocket *udp;

	try {
		udp = new CnUDPSocket(rba, port);
	} catch(const exception &e) {
		printf("Failed opening UDP socket?\n");
		return;
	}

	// in udp: out to all tcp
	// in tcp: out to udp
	
	bool gotfirst = false;
	vector<CnFDCtl *> clients;
	list<int> todel;

	while(gotfirst == false || clients.size() != 0) {
		struct pollfd *fds = (struct pollfd *)malloc(sizeof(struct pollfd)*(2+clients.size()));

		fds[0].fd = udp->get_fd();
		fds[0].events = POLLIN;
		if(udp->WriteQueueSize()) fds[0].events |= POLLOUT;

		fds[1].fd = serv->get_fd();
		fds[1].events = POLLIN;

		for(int i=0; i<clients.size(); i++) {
			fds[2+i].fd = clients[i]->get_fd();
			fds[2+i].events = POLLIN;
			if(clients[i]->WriteBufLen()) fds[2+i].events |= POLLOUT;
		}

		int pr = poll(fds, 2+clients.size(), -1);

		if(fds[0].revents & POLLIN) {
			// received udp
			udp->DirectRead();
			//printf("UDP received\n");

			ndatagram_t dg;
			while(udp->ReadQueueSize()) {
				dg = udp->Read();
				bom s = dg.serialize();
				for(int i=0; i<clients.size(); i++) {
					CnFDCtl &cfd = *clients[i];
					int size = s.size();
					cfd.AppendWriteQueue((char *)&size, sizeof(int));
					cfd.AppendWriteQueue(s);
				}
			}
		}

		if(fds[0].revents & POLLOUT) {
			udp->DirectWrite();
			//printf("UDP written\n");
		}
		
		for(int i=0; i<clients.size(); i++) {
			if(fds[2+i].revents & POLLHUP) {
				printf("Disconnecting client, HUP\n");
				todel.push_back(i);
				continue;
			}

			if(fds[2+i].revents & POLLIN) {
				try {
					clients[i]->DirectRead();
					if(clients[i]->getEOF()) {
						printf("Disconnecting client, EOF\n");
						todel.push_back(i);
					}
				} catch(const exception &e) {
					printf("Disconnecting client: %s\n", e.what());
					todel.push_back(i);
				}
				CnFDCtl &cfd = *clients[i];

				while(cfd.ReadBufSize() > sizeof(int)) {
					//printf("Received data\n");
					bom b = cfd.Peek(sizeof(int));
					int s; 
					memcpy(&s, b.p(), sizeof(int));
					//printf("s=%i %i\n", s, cfd.ReadBufSize());

					if(cfd.ReadBufSize() >= sizeof(int)+s) {
						//printf("Got enough to TX packet\n");
						// packet!
						cfd.Read(sizeof(int));
						bom packet = cfd.Read(s);
						ndatagram_t pkt(packet); //deserialize
						pkt.to = udp->other();
						udp->AppendWriteQueue(pkt);
					} else break;
				}
			}

			if(fds[2+i].revents & POLLOUT) {
				clients[i]->DirectWrite();
			}
		}

		if(fds[1].revents & POLLIN) {
			CnFDCtl *fd;
			try {
				fd = serv->ReceiveConnectionDirect();
				printf("Received connection on internal server\n");
				clients.push_back(fd);
				gotfirst = true;
			} catch(const exception &e) {
				printf("Failed to receive connection\n");
			}
		}
	
		for(list<int>::reverse_iterator it = todel.rbegin(); it != todel.rend(); it++) {
			printf("removing client %i\n", *it);
			clients.erase(clients.begin()+(*it));
		}

		todel.clear();
		free(fds);
	}

	printf("Ran out of clients\n");

	delete udp;
	delete serv;

	exit(0);
}

