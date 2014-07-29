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
//   http://www.gnu.org/licenses/lgpl.txt  This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
////////////////////////////////////////////////////////////////////////////
//
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//

#ifndef _XPCGetProtocol
#define _XPCGetProtocol

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
#ifdef __linux
    struct protoent protocol;
#endif
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
