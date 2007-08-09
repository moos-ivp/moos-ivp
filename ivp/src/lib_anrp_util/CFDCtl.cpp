/*************************************************************************
* CFDCtl.cpp - 
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


#include "CFDCtl.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "mymemmem.h"

void CFDCtl::CommonInit()
{
	// assume default blocksize
	this->is_open = false;
	this->fd = -1;
	
	rdBuf = wrBuf = NULL;
	rdBufLen = wrBufLen = 0;
	
	this->blocksize = 16384;

	wrMaxLen = -1;

	isEOF = false;
}

CFDCtl::CFDCtl()
{
	CommonInit();
}

CFDCtl::CFDCtl(int fd, int blocksize)
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

CFDCtl::~CFDCtl()
{
	if (is_open)
		Close();
}

int CFDCtl::SetNonBlockingMode(void)
{
	return 0;
}

int CFDCtl::Close(void)
{
	AllQueueFlush();
	is_open = false;
	close(fd); // ...
	return 0;
}

int CFDCtl::NonBlockingRead(void)
{
	return BlockingRead(0);
}

int CFDCtl::BlockingRead(int timeout)
{
	if (!is_open)
		return -1;

	fd_set rfds;

	int amtRd;

	char *buf = (char *)malloc(blocksize);

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

int CFDCtl::DirectRead(void)
{
	int amtRd;
	char *buf = (char *)malloc(blocksize);

	rdM.Lock();
	amtRd = read(fd, buf, blocksize);
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
		rdBuf = (char *)realloc((void *)rdBuf, rdBufLen + amtRd);
		memcpy(rdBuf + rdBufLen, buf, amtRd);
		rdBufLen += amtRd;
		rdM.UnLock();
		free(buf);
		return amtRd;
	}
}

int CFDCtl::DurationRead(int timelen)
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

int CFDCtl::ReadUntilChar(char c, int timeout)
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

int CFDCtl::ReadUntilStr(int l, char *s, int timeout)
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

int CFDCtl::ReadUntilLen(int l, int timeout)
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

int CFDCtl::NonBlockingWrite(void)
{
	return BlockingWrite(0);
}


int CFDCtl::BlockingWrite(int timeout)
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

int CFDCtl::DirectWrite(void)
{
	int amtWr = 0;
	int mtw = wrMaxLen == -1 ? wrBufLen : wrMaxLen < wrBufLen ? wrMaxLen :
	          wrBufLen;

	wrM.Lock();
	amtWr = write(fd, wrBuf, mtw);
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
		memmove((void *)wrBuf, (void *)(wrBuf + amtWr), wrBufLen - amtWr);
		wrBuf = (char *)realloc((void *)wrBuf, wrBufLen - amtWr);
		wrBufLen -= amtWr;

		if (wrBufLen == 0)
			wrBuf = NULL;

		wrM.UnLock();

		return amtWr;
	}
}

int CFDCtl::FullQueueWrite(int timelen)
{
	if (timelen == 0) {
		while (wrBufLen > 0) {
			BlockingWrite(1000000);
		}
	} else {

		struct timeval now, now2;

		while (wrBufLen > 0 && timelen > 0) {
			gettimeofday(&now, NULL);
			BlockingWrite(timelen);
			gettimeofday(&now2, NULL);
			timelen -= ((now2.tv_sec - now.tv_sec) * 1000000) + (now2.tv_usec - now.tv_usec);
		}
	}

	return 0;
}

int CFDCtl::ReadBufSize(void)
{
	return rdBufLen;
}

char *CFDCtl::Read(int nb)
{
	if (nb > rdBufLen || nb < 1)
		return NULL;

	char *ptr = (char *)malloc(nb + 1);

	memcpy(ptr, rdBuf, nb);

	rdM.Lock();

	memmove(rdBuf, rdBuf + nb, rdBufLen - nb);

	rdBufLen -= nb;
	
	if(rdBufLen == 0) {
		free(rdBuf);
		rdBuf = NULL;
	} else {
		rdBuf = (char *)realloc((void *)rdBuf, rdBufLen);
	}

	ptr[nb] = 0;

	rdM.UnLock();

	return ptr;
}

char *CFDCtl::Peek(int nb)
{
	if (nb > rdBufLen || nb < 1)
		return NULL;

	char *ptr = (char *)malloc(nb);

	rdM.Lock();

	memcpy(ptr, rdBuf, nb);

	rdM.UnLock();

	return ptr;
}

int CFDCtl::FindCharIndex(char c)
{
	if (rdBufLen == 0)
		return -1;

	rdM.Lock();

	char *p = (char *)memchr((void *)rdBuf, c, rdBufLen);

	int pos = p - rdBuf;

	rdM.UnLock();

	if (p == NULL)
		return -1;
	else
		return pos;
}

int CFDCtl::FindStrIndex(int l, char *s)
{
	if (rdBufLen == 0)
		return -1;

	rdM.Lock();

	char *p = (char *)mymemmem(rdBuf, rdBufLen, s, l);

	int pos = p - rdBuf;

	rdM.UnLock();

	if (p == NULL)
		return -1;
	else
		return pos;
}

int CFDCtl::WriteBufSize(void)
{
	return wrBufLen;
}

int CFDCtl::AppendWriteQueue(const char *ptr, int len)
{
	if (ptr == NULL || len == 0)
		return -1;

	if (len == -1)
		len = strlen(ptr);

	wrM.Lock();

	wrBuf = (char *)realloc((void *)wrBuf, wrBufLen + len);

	memcpy(wrBuf + wrBufLen, ptr, len);

	wrBufLen += len;

	wrM.UnLock();

	return 0;
}

int CFDCtl::WriteQueueFlush(void)
{
	wrM.Lock();

	if (wrBufLen > 0) {
		wrBufLen = 0;
		free(wrBuf);
		wrBuf = NULL;
	}

	wrM.UnLock();

	return 0;
}

int CFDCtl::ReadQueueFlush(void)
{
	rdM.Lock();

	if (rdBufLen > 0) {
		rdBufLen = 0;
		free(rdBuf);
		rdBuf = NULL;
	}

	rdM.UnLock();

	return 0;
}

int CFDCtl::AllQueueFlush(void)
{
	return ReadQueueFlush() + WriteQueueFlush();
}

