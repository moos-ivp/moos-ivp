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
