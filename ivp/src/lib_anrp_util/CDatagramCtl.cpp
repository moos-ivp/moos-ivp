/*************************************************************************
* CDatagramCtl.cpp - 
*************************************************************************
* (c) 2004 Andrew Patrikalakis <anrp@cml3.mit.edu>                      *
*                                                                       *
* This program is free software; you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 2 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program; if not, write to the Free Software           *
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
*************************************************************************/


#include "CDatagramCtl.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "mymemmem.h"

CDatagramCtl::CDatagramCtl()
{
	// assume default blocksize
	this->is_open = false;
	this->fd = -1;

	isEOF = false;
}


CDatagramCtl::CDatagramCtl(int fd)
{
	CDatagramCtl();
	if (fd == -1) {
		throw;
	} else {
		this->fd = fd;
		this->is_open = true; // check?
		SetNonBlockingMode();
	}
}

CDatagramCtl::~CDatagramCtl()
{
	if (is_open)
		Close();
}

int CDatagramCtl::SetNonBlockingMode(void)
{
	// this should be overridden if necessary
	return 0;
}

int CDatagramCtl::Close(void)
{
	// this should be called if subclassed
	AllQueueFlush();
	is_open = false;
	close(fd);
	return 0;
}

int CDatagramCtl::NonBlockingRead(void)
{
	return BlockingRead(0);
}

int CDatagramCtl::BlockingRead(int timeout)
{
	if (!is_open)
		return -1;

	fd_set rfds;

	int amtRd;

	char *buf = (char *)malloc(65536);

	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	tv.tv_sec = (timeout - (timeout % 1000000)) / 1000000;
	tv.tv_usec = timeout % 1000000;

	amtRd = select(fd + 1, &rfds, NULL, NULL, &tv);

	if (amtRd == -1) {
		amtRd = -1;
	} else if (amtRd) {
		amtRd = DirectRead();
	} else {
		amtRd = 0;
	}

	free(buf);

	return amtRd;
}

int CDatagramCtl::DirectRead(void)
{
	int amtRd;
	char *buf = (char *)malloc(65536);
	struct sockaddr r_their_addr;
	int r_addr_len;

	r_addr_len = sizeof(struct sockaddr);
	
	rdM.Lock();
	amtRd = recvfrom(fd, buf, 65536, 0, &r_their_addr, (socklen_t *)&r_addr_len);
	rdM.UnLock();

	if (amtRd == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
		errno = 0;
		free(buf);
		return 0;
	} else if (amtRd == -1) {
		free(buf);
		return -1;
	} else if (amtRd == 0) {
		free(buf);
		isEOF = true;
		return 0;
	} else {
		rdM.Lock();
		datagram_t tmp;
		memcpy(&(tmp.from), &r_their_addr, r_addr_len);
		tmp.len = amtRd;
		tmp.data = buf;
		rdBuf.push(tmp);
		rdM.UnLock();
		return amtRd;
	}
}

int CDatagramCtl::DurationRead(int timelen)
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

int CDatagramCtl::ReadUntilPacket(int timeout)
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

int CDatagramCtl::NonBlockingWrite(void)
{
	return BlockingWrite(0);
}


int CDatagramCtl::BlockingWrite(int timeout)
{
	if (!is_open)
		return -1;

	fd_set wfds;

	int amtWr;

	struct timeval tv;

	FD_ZERO(&wfds);

	FD_SET(fd, &wfds);

	tv.tv_sec = (timeout - (timeout % 1000000)) / 1000000;

	tv.tv_usec = timeout % 1000000;

	amtWr = select(fd + 1, NULL, &wfds, NULL, &tv);

	if (amtWr == -1) {
		amtWr = -1;
	} else if (amtWr) {
		amtWr = DirectWrite();
	} else {
		amtWr = 0;
	}

	return amtWr;
}

int CDatagramCtl::DirectWrite(void)
{
	int amtWr = 0;

	wrM.Lock();
	datagram_t &tmp = wrBuf.front();
	amtWr = sendto(fd, tmp.data, tmp.len, 0, (struct sockaddr *)&(tmp.to), sizeof(struct sockaddr));
	wrM.UnLock();

	if (amtWr == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
		errno = 0;
		return 0;
	} else if (amtWr == -1) {
		return -1;
	} else if (amtWr == 0) {
		return 0;
	} else {
		wrM.Lock();
		wrBuf.pop();
		wrM.UnLock();

		return amtWr;
	}
}

int CDatagramCtl::FullQueueWrite(int timelen)
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

int CDatagramCtl::ReadQueueSize(void)
{
	return rdBuf.size();
}

datagram_t CDatagramCtl::Read()
{
	rdM.Lock();
	datagram_t tmp = rdBuf.front();
	rdBuf.pop();
	rdM.UnLock();

	return tmp;
}

datagram_t CDatagramCtl::Peek()
{
	rdM.Lock();
	datagram_t tmp = rdBuf.front();
	rdM.UnLock();

	return tmp;
}

int CDatagramCtl::WriteQueueSize(void)
{
	return wrBuf.size();
}

int CDatagramCtl::AppendWriteQueue(datagram_t tmp)
{
	wrM.Lock();
	wrBuf.push(tmp);
	wrM.UnLock();

	return 0;
}

int CDatagramCtl::WriteQueueFlush(void)
{
	wrM.Lock();
	while(wrBuf.size()) {
		wrBuf.pop();
	}
	wrM.UnLock();

	return 0;
}

int CDatagramCtl::ReadQueueFlush(void)
{
	rdM.Lock();
	while(rdBuf.size()) {
		rdBuf.pop();
	}
	rdM.UnLock();

	return 0;
}

int CDatagramCtl::AllQueueFlush(void)
{
	return ReadQueueFlush() + WriteQueueFlush();
}

