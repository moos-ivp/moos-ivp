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
/***************************************************************************
                          MOOSLock.h  -  description
                             -------------------
    begin                : Mon Dec 18 2000
    copyright            : (C) 2000 by pnewman
    email                : pnewman@mit.edu
 ***************************************************************************/


// MOOSLock.h: interface for the CMOOSLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSLock_H__85AEC656_8C4D_4D1B_BCF2_9814B2611F9E__INCLUDED_)
#define AFX_MOOSLock_H__85AEC656_8C4D_4D1B_BCF2_9814B2611F9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//some conditional stuff..
#ifdef _WIN32
    #include "windows.h"
    #include "winbase.h"
    #include "winnt.h"
#else
    #include <pthread.h>
#endif

/** A very simple cross platform posix and win32 compatible mutex class*/
class CMOOSLock  
{
public:
    ///call this to unlock 
    void UnLock();

    ///call this to lock 
    void Lock();

    //default constructor has object unlocked intially
    CMOOSLock(bool bInitial = true);
    virtual ~CMOOSLock();

protected:
#ifdef _WIN32
    /// Win32 handle to locked object
    HANDLE            m_hLock;
#else
    /// posix mutex 
    pthread_mutex_t    m_hLock;
#endif




};

#endif // !defined(AFX_MOOSLock_H__85AEC656_8C4D_4D1B_BCF2_9814B2611F9E__INCLUDED_)
