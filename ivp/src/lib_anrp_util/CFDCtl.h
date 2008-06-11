/*************************************************************************
* CFDCtl.h - 
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


#ifndef __CFDCtl_h__
#define __CFDCtl_h__

#include <unistd.h>
#include <MOOSLock.h>

class CFDCtl
{

	protected:
		int fd;
		bool is_open;
		bool isEOF;

		CMOOSLock rdM;
		char *rdBuf;
		int rdBufLen;

		CMOOSLock wrM;
		char *wrBuf;
		int wrBufLen;
		int wrMaxLen;

		int blocksize;

		void CommonInit();

	public:

		CFDCtl(int fd, int blocksize = 16384);
		CFDCtl();
		virtual ~CFDCtl();

		virtual int SetNonBlockingMode();
		virtual int Close();

		int get_fd()
		{
			return fd;
		}

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
		void SetPortMaxWriteLen(int len)
		{
			wrMaxLen = len;
		}

		int ReadBufSize(void);
		int ReadBufLen(void) { return ReadBufSize(); }
		char *Read(int nb);
		char *Peek(int nb);

		int FindCharIndex(char c);
		int FindStrIndex(int l, char *s);

		int WriteBufSize(void);
		int WriteBufLen(void) { return WriteBufSize(); }
		int AppendWriteQueue(const char *ptr, int len = -1);

		int WriteQueueFlush(void);
		int ReadQueueFlush(void);
		int AllQueueFlush(void);
};

#endif /* __CFDCtl_h__ */
