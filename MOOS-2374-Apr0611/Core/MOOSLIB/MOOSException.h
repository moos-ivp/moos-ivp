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
//   This file is part of a  MOOS Core Component. 
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
//////////////////////////    END_GPL    //////////////////////////////////
// MOOSException.h: interface for the CMOOSException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSEXCEPTION_H__2EC1612C_E571_4BD9_BD5A_15473203F0D1__INCLUDED_)
#define AFX_MOOSEXCEPTION_H__2EC1612C_E571_4BD9_BD5A_15473203F0D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
