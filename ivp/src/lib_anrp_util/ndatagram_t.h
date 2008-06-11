#ifndef __ndatagram_t_h__
#define __ndatagram_t_h__

#include <unistd.h>
#include <MOOSLock.h>
#include <queue>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "bom.h"

struct ndatagram_t
{
	ndatagram_t() {
		memset(&from, 0, sizeof(struct sockaddr_in));
		memset(&to, 0, sizeof(struct sockaddr_in));
	}

	ndatagram_t(bom &in) { // deserialize
		memcpy(&from, in+0, sizeof(struct sockaddr_in));
		memcpy(&to, in+sizeof(struct sockaddr_in), sizeof(struct sockaddr_in));
		int s;
		memcpy(&s, in+(2*sizeof(struct sockaddr_in)), sizeof(int));
		data.resize(s);
		memcpy(data.p(), in+(2*sizeof(struct sockaddr_in))+sizeof(int), s);
	}

	struct sockaddr_in from;
	struct sockaddr_in to;
	bom data;

	bom serialize(void) {
		bom mem(sizeof(struct sockaddr_in) + sizeof(struct sockaddr_in) + sizeof(int) + data.size());
		memcpy(mem+0, &from, sizeof(struct sockaddr_in));
		memcpy(mem+sizeof(struct sockaddr_in), &to, sizeof(struct sockaddr_in));
		int s = data.size();
		memcpy(mem+(2*sizeof(struct sockaddr_in)), &s, sizeof(int));
		memcpy(mem+(2*sizeof(struct sockaddr_in))+sizeof(int), data.p(), data.size());
		
		return mem;
	}


};

#endif /* __ndatagram_t_h__ */
