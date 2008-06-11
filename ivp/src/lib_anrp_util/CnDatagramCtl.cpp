#include "CnDatagramCtl.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "mymemmem.h"

#include <string.h>
#include <errno.h>
#include <stdexcept>
#include "ssp.h"

using namespace std;

CnDatagramCtl::CnDatagramCtl()
{
	this->is_open = false;
	this->fd = -1;

	isEOF = false;
}


CnDatagramCtl::CnDatagramCtl(int fd)
{
	CnDatagramCtl();
	if (fd == -1) {
		throw;
	} else {
		this->fd = fd;
		this->is_open = true; // check?
		SetNonBlockingMode();
	}
}

CnDatagramCtl::~CnDatagramCtl()
{
	if (is_open)
		Close();
}

int CnDatagramCtl::SetNonBlockingMode(void)
{
	// this should be overridden if necessary
	return 0;
}

int CnDatagramCtl::Close(void)
{
	// this should be called if subclassed
	AllQueueFlush();
	is_open = false;
	close(fd);
	return 0;
}

int CnDatagramCtl::NonBlockingRead(void)
{
	return BlockingRead(0);
}

int CnDatagramCtl::BlockingRead(int timeout)
{
	if (!is_open)
		return -1;

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
	} else { // nothing to do
	}

	return amtRd;
}

int CnDatagramCtl::DirectRead(void)
{
	int amtRd;
	bom buf(65536);
	struct sockaddr r_their_addr;
	int r_addr_len;

	r_addr_len = sizeof(struct sockaddr);
	
	rdM.Lock();
	amtRd = recvfrom(fd, buf.p(), buf.size(), 0, &r_their_addr, (socklen_t *)&r_addr_len);
	rdM.UnLock();

	if (amtRd == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
		errno = 0;
		return 0;
	} else if (amtRd == -1) {
		throw runtime_error(ssp("Error on recvfrom(fd = %i): %s", fd, strerror(errno)));
	} else if (amtRd == 0) {
		isEOF = true;
		return 0;
	} else {
		rdM.Lock();
		ndatagram_t tmp;
		memcpy(&(tmp.from), &r_their_addr, r_addr_len);
		tmp.data = buf;
		rdBuf.push(tmp);
		rdM.UnLock();
		return amtRd;
	}
}

int CnDatagramCtl::DurationRead(int timelen)
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

int CnDatagramCtl::ReadUntilPacket(int timeout)
{

	struct timeval now, now2;
	int rval = 0;

	while (timeout > 0) {
		gettimeofday(&now, NULL);
		rval += BlockingRead(timeout);
		gettimeofday(&now2, NULL);
		timeout -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
		if(rval >= 0) break;
	}

	return rval;
}

int CnDatagramCtl::NonBlockingWrite(void)
{
	return BlockingWrite(0);
}

int CnDatagramCtl::BlockingWrite(int timeout)
{
	if (!is_open) throw logic_error("Attempted to write to closed CnDatagramCtl");

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
	} else { // nothing to do
	}

	return amtWr;
}

int CnDatagramCtl::DirectWrite(void)
{
	int amtWr = 0;

	wrM.Lock();
	ndatagram_t &tmp = wrBuf.front();
	amtWr = sendto(fd, tmp.data.p(), tmp.data.size(), 0, (struct sockaddr *)&(tmp.to), sizeof(struct sockaddr));

	if (amtWr == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
		errno = 0;
		wrM.UnLock();
		return 0;
	} else if (amtWr == -1) {
		wrM.UnLock();
		throw runtime_error(ssp("Error on sendto(fd = %i): %s", fd, strerror(errno)));
	} else if (amtWr == 0) {
		wrM.UnLock();
		return 0;
	} else {
		wrBuf.pop();
		wrM.UnLock();

		return amtWr;
	}
}

int CnDatagramCtl::FullQueueWrite(int timelen)
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

int CnDatagramCtl::ReadQueueSize(void)
{
	return rdBuf.size();
}

ndatagram_t CnDatagramCtl::Read()
{
	rdM.Lock();
	ndatagram_t tmp = rdBuf.front();
	rdBuf.pop();
	rdM.UnLock();

	return tmp;
}

ndatagram_t CnDatagramCtl::Peek()
{
	rdM.Lock();
	ndatagram_t tmp = rdBuf.front();
	rdM.UnLock();

	return tmp;
}

int CnDatagramCtl::WriteQueueSize(void)
{
	return wrBuf.size();
}

int CnDatagramCtl::AppendWriteQueue(ndatagram_t tmp)
{
	wrM.Lock();
	wrBuf.push(tmp);
	wrM.UnLock();

	return 0;
}

int CnDatagramCtl::WriteQueueFlush(void)
{
	wrM.Lock();
	while(wrBuf.size()) {
		wrBuf.pop();
	}
	wrM.UnLock();

	return 0;
}

int CnDatagramCtl::ReadQueueFlush(void)
{
	rdM.Lock();
	while(rdBuf.size()) {
		rdBuf.pop();
	}
	rdM.UnLock();

	return 0;
}

int CnDatagramCtl::AllQueueFlush(void)
{
	return ReadQueueFlush() + WriteQueueFlush();
}

