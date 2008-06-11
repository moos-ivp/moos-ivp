/*************************************************************************
* CClientTCPSocket.cpp - 
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
#include "CClientTCPSocket.h"

#include <stdio.h>
#include "ResolverLock.h"

using namespace std;

CClientTCPSocket::CClientTCPSocket(string hostname, int port)
{
	this->port = port;
	this->hostname = hostname;
	LockResolver();
	struct hostent *he = gethostbyname(hostname.c_str());
	struct sockaddr_in their_addr;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(port);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	UnlockResolver();
	memset(&(their_addr.sin_zero), 0, 8);

	if(connect(sfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
		close(sfd);
		throw "failure to connect"; 
	}

	fd = sfd;
	is_open = true;
	SetNonBlockingMode();
}

CClientTCPSocket::~CClientTCPSocket()
{
	if (is_open) {
		Close();
	}
}

