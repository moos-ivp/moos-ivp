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
//   http://www.gnu.org/licenses/lgpl.txt
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
////////////////////////////////////////////////////////////////////////////
**/


//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//
//////////////////////////    END_GPL    //////////////////////////////////
#include "MOOS/libMOOS/Comms/XPCGetProtocol.h"
#include "MOOS/libMOOS/Utils/MOOSScopedLock.h"

CMOOSLock _ProtocolLock;

XPCGetProtocol::XPCGetProtocol(const char *_sName)
{
    MOOS::ScopedLock L(_ProtocolLock);

#ifdef UNIX
       cIteratorFlag = 0;
#endif

       // Retrieves the protocol structure by name
       protocolPtr = getprotobyname(_sName);

}

XPCGetProtocol::XPCGetProtocol(int _iProtocol)
{
        MOOS::ScopedLock L(_ProtocolLock);

#ifdef UNIX
        cIteratorFlag = 0;
#endif

        // Retrieves the protocol structure by number
        protocolPtr = getprotobynumber(_iProtocol);
        if (protocolPtr == NULL)
        {
              XPCException exceptObject("Could Not Get Protocol By Number");
              throw exceptObject;
              return;
        }
}

XPCGetProtocol::~XPCGetProtocol()
{
    MOOS::ScopedLock L(_ProtocolLock);
#ifdef UNIX
        endprotoent();
#endif
}


#ifdef UNIX
    void XPCGetProtocol::vOpenProtocolDb()
    {
        MOOS::ScopedLock L(_ProtocolLock);

        endprotoent();
        cIteratorFlag = 1;
        setprotoent(1);
    }

    // Iterates through the list of protocols
    char XPCGetProtocol::cGetNextProtocol()
    {
        MOOS::ScopedLock L(_ProtocolLock);

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



