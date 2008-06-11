/*************************************************************************
* CDatagramCtl.h - 
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


#ifndef __CDatagramCtl_h__
#define __CDatagramCtl_h__

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <MOOSLock.h>
#include <queue>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

class datagram_t
{
	public:
		datagram_t() {
			data = NULL;
			len = 0;
			memset(&from, 0, sizeof(struct sockaddr_in));
			memset(&to, 0, sizeof(struct sockaddr_in));
		}

		datagram_t(const datagram_t &ref) {
			len = ref.len;
			from = ref.from;
			to = ref.to;
			if(len != 0 && ref.data != NULL) {
				data = (char *)malloc(len);
				memcpy(data, ref.data, len);
			}
		}
		
		~datagram_t() {
			if(len != 0 && data != NULL) {
				free(data);
			}
		}
	
		struct sockaddr_in from;
		struct sockaddr_in to;
		int len;
		char *data;
};

class CDatagramCtl
{

	protected:
		int fd;
		bool is_open;
		bool isEOF;

		CMOOSLock rdM;
		queue<datagram_t> rdBuf;

		CMOOSLock wrM;
		queue<datagram_t> wrBuf;

	public:
		CDatagramCtl(int fd);
		CDatagramCtl();
		virtual ~CDatagramCtl();

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
		int ReadUntilPacket(int timeout = 100000);
		int DirectRead(void);

		int NonBlockingWrite(void);
		int BlockingWrite(int timeout = 100000);
		int FullQueueWrite(int timelen = 100000);
		int DirectWrite(void);

		int ReadQueueSize(void);
		datagram_t Read();
		datagram_t Peek();

		int WriteQueueSize(void);
		int AppendWriteQueue(datagram_t);

		int WriteQueueFlush(void);
		int ReadQueueFlush(void);
		int AllQueueFlush(void);
};

#endif /* __CDatagramCtl_h__ */
