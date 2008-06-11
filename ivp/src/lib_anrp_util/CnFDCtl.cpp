#include "CnFDCtl.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "bom.h"
#include <stdexcept>
#include "mymemmem.h"
#include "ssp.h"

using namespace std;

void CnFDCtl::CommonInit()
{
	// assume default blocksize
	this->is_open = false;
	this->fd = -1;
	
	this->blocksize = 16384;

	wrMaxLen = -1;

	isEOF = false;
}

CnFDCtl::CnFDCtl()
{
	CommonInit();
}

CnFDCtl::CnFDCtl(int fd, int blocksize)
{
	CommonInit();
	if (fd == -1) {
		throw;
	} else {
		this->fd = fd;
		this->is_open = true; // check?
		SetNonBlockingMode();
	}

	if (blocksize < 1)
		blocksize = 1;

	this->blocksize = blocksize;
}

CnFDCtl::~CnFDCtl()
{
	if (is_open)
		Close();
}

int CnFDCtl::SetNonBlockingMode(void)
{
	return 0;
}

int CnFDCtl::Close(void)
{
	AllQueueFlush();
	is_open = false;
	close(fd); // ...
	return 0;
}

int CnFDCtl::NonBlockingRead(void)
{
	return BlockingRead(0);
}

int CnFDCtl::BlockingRead(int timeout)
{
	if (!is_open) throw logic_error("Attempted to read from non-open CnFDCtl");

	fd_set rfds;

	int amtRd;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	tv.tv_sec = (timeout - (timeout % 1000000)) / 1000000;
	tv.tv_usec = timeout % 1000000;

	amtRd = select(fd + 1, &rfds, NULL, NULL, &tv);

	if (amtRd == -1) {
		throw runtime_error(ssp("Error on select(fd = %i): %s", fd, strerror(errno)));
	} else if (amtRd) {
		amtRd = DirectRead();
	} else { // amdRd == 0, do nothing
	}

	return amtRd;
}

int CnFDCtl::DirectRead(void)
{
	int amtRd;
	bom buf(blocksize);

	amtRd = read(fd, buf.p(), blocksize);

	if (amtRd == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
		errno = 0;
		return 0;
	} else if (amtRd == -1) {
		throw runtime_error(ssp("Error reading on fd %i: %s", fd, strerror(errno)));
	} else if (amtRd == 0) {
		isEOF = true;
		return 0;
	} else {
		rdM.Lock();
		int oldl = rdBuf.size();
		rdBuf.resize(rdBuf.size() + amtRd);
		memcpy(rdBuf + oldl, buf.p(), amtRd);
		rdM.UnLock();
		return amtRd;
	}
}

int CnFDCtl::DurationRead(int timelen)
{
	struct timeval now, now2;
	int rval = 0;

	while (timelen > 0) {
		gettimeofday(&now, NULL);
		rval += BlockingRead(timelen);
		gettimeofday(&now2, NULL);
		timelen -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
	}

	return rval;
}

int CnFDCtl::ReadUntilChar(char c, int timeout)
{
	struct timeval now, now2;
	int rval = 0;

	while (timeout > 0 && FindCharIndex(c) == -1) {
		gettimeofday(&now, NULL);
		rval += BlockingRead(timeout);
		gettimeofday(&now2, NULL);
		timeout -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
	}

	return FindCharIndex(c);
}

int CnFDCtl::ReadUntilStr(int l, char *s, int timeout)
{

	struct timeval now, now2;
	int rval = 0;

	while (timeout > 0 && FindStrIndex(l, s) == -1) {
		gettimeofday(&now, NULL);
		rval += BlockingRead(timeout);
		gettimeofday(&now2, NULL);
		timeout -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
	}

	return FindStrIndex(l, s);
}

int CnFDCtl::ReadUntilLen(int l, int timeout)
{
	struct timeval now, now2;
	int rval = 0;

	while(timeout > 0 && ReadBufSize() < l) {
		gettimeofday(&now, NULL);
		rval += BlockingRead(timeout);
		gettimeofday(&now2, NULL);
		timeout -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
	}

	return rval;
}

int CnFDCtl::NonBlockingWrite(void)
{
	return BlockingWrite(0);
}


int CnFDCtl::BlockingWrite(int timeout)
{
	if (!is_open) throw logic_error("Attempted to write to CnFDCtl that wasn't open");

	fd_set wfds;

	int amtWr;
	struct timeval tv;

	FD_ZERO(&wfds);
	FD_SET(fd, &wfds);
	tv.tv_sec = (timeout - (timeout % 1000000)) / 1000000;
	tv.tv_usec = timeout % 1000000;

	amtWr = select(fd + 1, NULL, &wfds, NULL, &tv);

	if (amtWr == -1) {
		throw runtime_error(ssp("Error on select[write](fd = %i): %s", fd, strerror(errno)));
	} else if (amtWr) {
		amtWr = DirectWrite();
	} else { // amtWr == 0, nothing to do
	}

	return amtWr;
}

int CnFDCtl::DirectWrite(void)
{
	int amtWr = 0;

	wrM.Lock();
	int mtw = wrMaxLen == -1 ? wrBuf.size() : 
	          wrMaxLen < wrBuf.size() ? wrMaxLen :
	          wrBuf.size();

	amtWr = write(fd, wrBuf.p(), mtw);

	if (amtWr == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
		errno = 0;
		wrM.UnLock();
		return 0;
	} else if (amtWr == -1) {
		wrM.UnLock();
		throw runtime_error(ssp("Error on write(fd = %i): %s", fd, strerror(errno)));
	} else if (amtWr == 0) {
		wrM.UnLock();
		return 0;
	} else {
		memmove(wrBuf.p(), wrBuf + amtWr, wrBuf.size() - amtWr);
		wrBuf.resize(wrBuf.size() - amtWr);
		wrM.UnLock();

		return amtWr;
	}
}

int CnFDCtl::FullQueueWrite(int timelen)
{
	if (timelen == 0) {
		while (wrBuf.size() > 0) {
			BlockingWrite(1000000);
		}
	} else {

		struct timeval now, now2;

		while (wrBuf.size() > 0 && timelen > 0) {
			gettimeofday(&now, NULL);
			BlockingWrite(timelen);
			gettimeofday(&now2, NULL);
			timelen -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
		}
	}

	return 0;
}

int CnFDCtl::ReadBufSize(void)
{
	return rdBuf.size();
}

bom CnFDCtl::Read(int nb)
{
	if (nb > rdBuf.size() || nb < 1)
		throw runtime_error(ssp("Invalid size passed to Read (%i), curlen %i", nb, rdBuf.size()));

	bom ptr(nb);

	rdM.Lock();
	memcpy(ptr.p(), rdBuf.p(), nb);
	memmove(rdBuf.p(), rdBuf + nb, rdBuf.size() - nb);

	rdBuf.resize(rdBuf.size() - nb);

	rdM.UnLock();

	return ptr;
}

bom CnFDCtl::Peek(int nb)
{
	if (nb > rdBuf.size() || nb < 1)
		throw runtime_error(ssp("Invalid size passed to Peek (%i), curlen %i", nb, rdBuf.size()));

	bom ptr(nb);

	rdM.Lock();
	memcpy(ptr.p(), rdBuf.p(), nb);
	rdM.UnLock();

	return ptr;
}

int CnFDCtl::FindCharIndex(char c)
{
	if (rdBuf.size() == 0)
		return -1;

	rdM.Lock();

	char *p = (char *)memchr(rdBuf.p(), c, rdBuf.size());

	int pos = p - rdBuf.c();

	rdM.UnLock();

	if (p == NULL)
		return -1;
	else
		return pos;
}

int CnFDCtl::FindStrIndex(int l, char *s)
{
	if (rdBuf.size() == 0)
		return -1;

	rdM.Lock();

	char *p = (char *)mymemmem(rdBuf.c(), rdBuf.size(), s, l);
	int pos = p - rdBuf.c();

	rdM.UnLock();

	if (p == NULL)
		return -1;
	else
		return pos;
}

int CnFDCtl::WriteBufSize(void)
{
	return wrBuf.size();
}

int CnFDCtl::AppendWriteQueue(const char *ptr, int len)
{
	if (ptr == NULL || len == 0)
		return -1;

	if (len == -1)
		len = strlen(ptr);

	wrM.Lock();

	int olds = wrBuf.size();
	wrBuf.resize(wrBuf.size() + len);

	memcpy(wrBuf + olds, ptr, len);

	wrM.UnLock();

	return 0;
}

int CnFDCtl::AppendWriteQueue(bom &b)
{
	return AppendWriteQueue(b.c(), b.size());
}

int CnFDCtl::WriteQueueFlush(void)
{
	wrM.Lock();

	wrBuf.resize(0);

	wrM.UnLock();

	return 0;
}

int CnFDCtl::ReadQueueFlush(void)
{
	rdM.Lock();

	rdBuf.resize(0);

	rdM.UnLock();

	return 0;
}

int CnFDCtl::AllQueueFlush(void)
{
	return ReadQueueFlush() + WriteQueueFlush();
}

