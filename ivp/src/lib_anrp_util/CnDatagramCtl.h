#ifndef __CnDatagramCtl_h__
#define __CnDatagramCtl_h__

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <MOOSLock.h>
#include <queue>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "bom.h"

#include "ndatagram_t.h"

class CnDatagramCtl
{

protected:
	int fd;
	bool is_open;
	bool isEOF;

	CMOOSLock rdM;
	std::queue<ndatagram_t> rdBuf;

	CMOOSLock wrM;
	std::queue<ndatagram_t> wrBuf;

public:
	CnDatagramCtl(int fd);
	CnDatagramCtl();
	virtual ~CnDatagramCtl();

	virtual int SetNonBlockingMode();
	virtual int Close();

	int get_fd() { return fd; }

	int getEOF() { return isEOF; }

	int NonBlockingRead(void);
	int BlockingRead(int timeout = 100000);
	int DurationRead(int timelen = 100000);
	int ReadUntilPacket(int timeout = 100000);
	int DirectRead(void);

	int NonBlockingWrite(void);
	int BlockingWrite(int timeout = 100000);
	int FullQueueWrite(int timelen = 100000);
	int DirectWrite(void);

	int ReadQueueSize(void);
	ndatagram_t Read();
	ndatagram_t Peek();

	int WriteQueueSize(void);
	int AppendWriteQueue(ndatagram_t);

	int WriteQueueFlush(void);
	int ReadQueueFlush(void);
	int AllQueueFlush(void);
};

#endif /* __CnDatagramCtl_h__ */
