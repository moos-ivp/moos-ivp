///////////////////////////////////////////////////////////////////////////
//
//   This file is part of the MOOS project
//
//   MOOS : Mission Oriented Operating Suite A suit of
//   Applications and Libraries for Mobile Robotics Research
//   Copyright (C) Paul Newman
//    
//   This software was written by Paul Newman at MIT 2001-2002 and
//   the University of Oxford 2003-2013
//
//   email: pnewman@robots.ox.ac.uk.
//
//   This source code and the accompanying materials
//   are made available under the terms of the GNU Lesser Public License v2.1
//   which accompanies this distribution, and is available at
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
//
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//

#ifndef _XPCGetHostInfo
#define _XPCGetHostInfo

#include "XPCException.h"
#ifdef UNIX
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#elif _WIN32
    #include <winsock2.h>
#else
    #error "Looks like the build scripts didn't set the platform type"
#endif
    
#ifndef in_addr_t
    #define in_addr_t unsigned long
#endif

enum hostType {NAME, ADDRESS};

class XPCGetHostInfo
{
#ifdef UNIX
    char cIteratorFlag;        // Host database iteration flag
#endif
    struct hostent *hostPtr;    // Entry within the host address database
public:
#ifdef UNIX
    // Default constructor.  Opens the host entry database
    XPCGetHostInfo()
    {
        vOpenHostDb();
    }
#endif
    // Retrieves the host entry based on the host name or address
    XPCGetHostInfo(const char *_sHostName, hostType _type);

    XPCGetHostInfo(in_addr_t *_netAddr);

    // Destructor.  Closes the host entry database.
    ~XPCGetHostInfo()
    {
#ifdef UNIX
        endhostent();
#endif
    }
#ifdef UNIX
    // Retrieves the next host entry in the database
    char cGetNextHost();

    // Opens the host entry database
    void vOpenHostDb()
    {
        endhostent();
        cIteratorFlag = 1;
        sethostent(1);
    }
#endif
    // Retrieves the hosts IP address
    char *sGetHostAddress() 
    {
        struct in_addr *addr_ptr;
        addr_ptr = (struct in_addr *)*hostPtr->h_addr_list;
        return inet_ntoa(*addr_ptr);
    }    
    
    // Retrieves the hosts name
    char *sGetHostName()
    {
        return hostPtr->h_name;
    }
};

#endif
