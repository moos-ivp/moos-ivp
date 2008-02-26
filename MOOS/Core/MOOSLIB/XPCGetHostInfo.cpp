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
#include "XPCGetHostInfo.h"


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
            return;
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
            return;
        }

        hostPtr = gethostbyaddr((char *)&netAddr, sizeof(netAddr), AF_INET);
        if (hostPtr == NULL)
        {
            XPCException exceptObject("Error Getting Host By Address");
            throw exceptObject;
            return;
        }
    }    
    else
    {
        XPCException exceptObject("Parameter Error Constructing XPCGetHostInfo");
        throw exceptObject;    
        return;
    }
}

XPCGetHostInfo::XPCGetHostInfo(in_addr_t *_netAddr)
{
    if (*_netAddr == INADDR_NONE)
    {
        XPCException exceptObject("Error Getting Host By Address");
        throw exceptObject;
        return;
    }

    hostPtr = gethostbyaddr((char *)_netAddr, sizeof(*_netAddr), AF_INET);
    if (hostPtr == NULL)
    {
        XPCException exceptObject("Error Getting Host By Address");
        throw exceptObject;
        return;
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
