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
// MOOSException.h: interface for the CMOOSException class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CMOOSExceptionH
#define CMOOSExceptionH

#include <string>

/** A trivial Exception class */
class CMOOSException  
{
public:


    CMOOSException();
    virtual ~CMOOSException();

    /** construct an exception with a string argument giving the reason
    for the exception*/
    CMOOSException(const char * sStr);
    CMOOSException(const std::string & s);

    char * c_str(){return m_sReason;}
            
    /// storage for the exception reason
    char m_sReason[100];

};

#endif // !defined(AFX_MOOSEXCEPTION_H__2EC1612C_E571_4BD9_BD5A_15473203F0D1__INCLUDED_)
