///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS CORE Component. 
//        
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//            
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//
//////////////////////////    END_GPL    //////////////////////////////////
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
