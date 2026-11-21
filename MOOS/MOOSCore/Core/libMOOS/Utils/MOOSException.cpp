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

// MOOSException.cpp: implementation of the CMOOSException class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
    #pragma warning(disable : 4503)
#endif

#include "MOOS/libMOOS/Utils/MOOSException.h"
#include <cstring>
#include <string>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSException::CMOOSException()
{

}

CMOOSException::~CMOOSException()
{

}

CMOOSException::CMOOSException(const std::string & s)
{
    strncpy(m_sReason,s.c_str(),sizeof(m_sReason)-1);
    m_sReason[sizeof(m_sReason)-1]='\0';
}


CMOOSException::CMOOSException(const char * sReason)
{
    strncpy(m_sReason,sReason,sizeof(m_sReason)-1);
    m_sReason[sizeof(m_sReason)-1]='\0';
}
