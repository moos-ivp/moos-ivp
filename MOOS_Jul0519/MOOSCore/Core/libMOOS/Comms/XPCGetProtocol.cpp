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

CMOOSLock XPCGetProtocol::_ProtocolLock;

XPCGetProtocol::XPCGetProtocol(const char *_sName)
{
    MOOS::ScopedLock L(_ProtocolLock);

    struct protoent* ent = getprotobyname(_sName);
    if (ent == NULL)
    {
        XPCException exceptObject("Could Not Get Protocol By Name");
        throw exceptObject;
    }
    _index = 0;
    _protocols.push_back(XPCGetProtocol::ProtoEnt(ent));
}

XPCGetProtocol::XPCGetProtocol(int _iProtocol)
{
    MOOS::ScopedLock L(_ProtocolLock);

    // Retrieves the protocol structure by number
    struct protoent* ent = getprotobynumber(_iProtocol);
    if (ent == NULL)
    {
        XPCException exceptObject("Could Not Get Protocol By Number");
        throw exceptObject;
        return;
    }
    _index = 0;
    _protocols.push_back(XPCGetProtocol::ProtoEnt(ent));
}

XPCGetProtocol::~XPCGetProtocol()
{
    MOOS::ScopedLock L(_ProtocolLock);
}


#ifdef UNIX
void XPCGetProtocol::vOpenProtocolDb()
{
    MOOS::ScopedLock L(_ProtocolLock);

    endprotoent();
    setprotoent(1);
    struct protoent* ent = 0;
    _index = -1;   // call cGetNextProtocol before accessing the first one...
    _protocols.clear();
    while ((ent = getprotoent())) {
        _protocols.push_back(XPCGetProtocol::ProtoEnt(ent));
    }
    endprotoent();
}

// Iterates through the list of protocols
char XPCGetProtocol::cGetNextProtocol()
{
    MOOS::ScopedLock L(_ProtocolLock);

    if (_index + 1 >= static_cast<int>(_protocols.size())) return 0;
    ++_index;
    return 1;
}
#endif

XPCGetProtocol::ProtoEnt::ProtoEnt(struct protoent const* ent):
    _name(ent? ent->p_name: ""),
    _number(ent? ent->p_proto: 0)
{
    if (ent == 0) return;
    for (char** alias = ent->p_aliases; *alias; ++alias) {
        _aliases.push_back(std::string(*alias));
    } 
}
XPCGetProtocol::ProtoEnt::~ProtoEnt()
{
}

std::string const& XPCGetProtocol::ProtoEnt::name() const {
    return _name;
}

int XPCGetProtocol::ProtoEnt::number() const {
    return _number;
}
