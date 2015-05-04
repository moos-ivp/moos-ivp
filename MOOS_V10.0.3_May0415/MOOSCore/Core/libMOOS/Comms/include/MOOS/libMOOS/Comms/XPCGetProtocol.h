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
#include "MOOS/libMOOS/Utils/MOOSScopedLock.h"
#include <string>
#include <vector>

class XPCGetProtocol
{
    private:
        static CMOOSLock _ProtocolLock;
        // Helper class: encapsulates a copy of the protoent structure
        class ProtoEnt {
            private:
                std::string _name;
                std::vector<std::string> _aliases;
                int _number;

            public:
                ProtoEnt(struct protoent const* ent);
                ~ProtoEnt();
                std::string const& name() const;
                int number() const;
        };
        int _index;
        std::vector<ProtoEnt> _protocols;
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

        // Destructor closes the database connection
        ~XPCGetProtocol();

#ifdef UNIX
        // Opens and reads the protocol database
        void vOpenProtocolDb();

        // Iterates through the list of protocols
        char cGetNextProtocol();
#endif

        // Returns the protocol name
        char const* sGetProtocolName() { return _index >= static_cast<int>(_protocols.size())? "": _protocols[_index].name().c_str(); }

        // Returns the protcol number
        int iGetProtocolNumber() { return _index >= static_cast<int>(_protocols.size())? 0: _protocols[_index].number(); }
};
#endif
