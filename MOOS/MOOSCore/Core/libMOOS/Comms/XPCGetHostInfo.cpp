/**
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
//   http://www.gnu.org/licenses/lgpl.txt distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/


//
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//
//////////////////////////    END_GPL    //////////////////////////////////
#include "MOOS/libMOOS/Comms/XPCGetHostInfo.h"


XPCGetHostInfo::XPCGetHostInfo(const char *_sHost, hostType _type)
{
#ifdef UNIX
    cIteratorFlag = 0;
#endif
    if (_type == NAME)
    {
        // Retrieve host by name
        hostPtr = gethostbyname(_sHost);
        if (hostPtr == NULL)
        {
            XPCException exceptObject("Error Getting Host By Name");
            throw exceptObject;
        }
    }    
    else if (_type == ADDRESS)
    {
        // Retrieve host by address
        //
        in_addr_t  netAddr = inet_addr(_sHost);
        if (netAddr == INADDR_NONE)
        {
            XPCException exceptObject("Error Getting Host By Address");
            throw exceptObject;
        }

        hostPtr = gethostbyaddr((char *)&netAddr, sizeof(netAddr), AF_INET);
        if (hostPtr == NULL)
        {
            XPCException exceptObject("Error Getting Host By Address");
            throw exceptObject;
        }
    }    
    else
    {
        XPCException exceptObject("Parameter Error Constructing XPCGetHostInfo");
        throw exceptObject;    
    }
}

XPCGetHostInfo::XPCGetHostInfo(in_addr_t *_netAddr)
{
    if (*_netAddr == INADDR_NONE)
    {
        XPCException exceptObject("Error Getting Host By Address");
        throw exceptObject;
    }

    hostPtr = gethostbyaddr((char *)_netAddr, sizeof(*_netAddr), AF_INET);
    if (hostPtr == NULL)
    {
        XPCException exceptObject("Error Getting Host By Address");
        throw exceptObject;
    }
}

#ifdef UNIX
char XPCGetHostInfo::cGetNextHost()
{
     // Get the next host from the database
     if (cIteratorFlag == 1)
     {
         if ((hostPtr = gethostent()) == NULL)
              return 0;
         else
              return 1;
     }
     return 0;
}
#endif
