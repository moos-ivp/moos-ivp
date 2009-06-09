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
#ifndef _XPCGetProtocol
#define _XPCGetprotocol

#ifdef UNIX
    #include <netdb.h>
#elif _WIN32
    #include <winsock2.h>
#else
    #error "Looks like the build scripts didn't set the platform type"
#endif

#include "XPCException.h"

class XPCGetProtocol
{
#ifdef UNIX
    char cIteratorFlag;        // Protocol database iteration flag
#endif
    struct protoent *protocolPtr;    // Pointer to protocol database entry
public:
#ifdef UNIX
    // Default constructor.  Opens the protocol database
    XPCGetProtocol()
    {
        vOpenProtocolDb();
    }
#endif

    // Constructor.  Returns the protoent structure given the protocol name.
    XPCGetProtocol(const char *_sName);

    // Constructor.  Returns the protoent structure given the protocol number
    XPCGetProtocol(int _iProtocol);

    // Desstructor closes the database connection  
        ~XPCGetProtocol()
        {
#ifdef UNIX
                endprotoent();
#endif
        }

    // Opens the protocol database and sets the cIteratorFlag to true
#ifdef UNIX
    void vOpenProtocolDb()
    {
        endprotoent();
        cIteratorFlag = 1;
        setprotoent(1);
    }    

    // Iterates through the list of protocols
    char cGetNextProtocol()
    {
        if (cIteratorFlag == 1)
        {
            if ((protocolPtr = getprotoent()) == NULL)
                return 0;
            else
                return 1;
        }
        return 0;
    } 
#endif

    // Returns the protocol name
    char *sGetProtocolName() { return protocolPtr->p_name; }

    // Returns the protcol number
    int iGetProtocolNumber() { return protocolPtr->p_proto; }
};

#endif
