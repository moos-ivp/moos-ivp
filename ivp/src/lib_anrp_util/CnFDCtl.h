#ifndef __CnFDCtl_h__
#define __CnFDCtl_h__

#include <unistd.h>
#include <MOOSLock.h>
#include "bom.h"

class CnFDCtl
{
protected:
	int fd;
	bool is_open;
	bool isEOF;

	CMOOSLock rdM;
	bom rdBuf;

	CMOOSLock wrM;
	bom wrBuf;
	int wrMaxLen;

	int blocksize;

	void CommonInit();

public:

	CnFDCtl(int fd, int blocksize = 16384);
	CnFDCtl();
	virtual ~CnFDCtl();

	virtual int SetNonBlockingMode();
	virtual int Close();

	int get_fd() { return fd; }

	int getEOF() { return isEOF; }

	int NonBlockingRead(void);
	int BlockingRead(int timeout = 100000);
	int DurationRead(int timelen = 100000);
	int ReadUntilChar(char c, int timeout = 100000);
	int ReadUntilStr(int l, char *s, int timeout = 100000);
	int ReadUntilLen(int l, int timeout = 100000);
	int DirectRead(void);

	int NonBlockingWrite(void);
	int BlockingWrite(int timeout = 100000);
	int FullQueueWrite(int timelen = 100000);
	int DirectWrite(void);
	void SetPortMaxWriteLen(int len) { wrMaxLen = len; }

	int ReadBufSize(void);
	int ReadBufLen(void) { return ReadBufSize(); }
	bom Read(int nb);
	bom Peek(int nb);

	int FindCharIndex(char c);
	int FindStrIndex(int l, char *s);

	int WriteBufSize(void);
	int WriteBufLen(void) { return WriteBufSize(); }
	int AppendWriteQueue(const char *ptr, int len = -1);
	int AppendWriteQueue(bom &b);

	int WriteQueueFlush(void);
	int ReadQueueFlush(void);
	int AllQueueFlush(void);
};

#endif /* __CnFDCtl_h__ */
