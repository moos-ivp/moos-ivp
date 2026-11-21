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

/***************************************************************************
                          CMLLock.cpp  -  description
                             -------------------
    begin                : Mon Dec 18 2000
    copyright            : (C) 2000 by pnewman
    email                : pnewman@mit.edu
 ***************************************************************************/


// MOOSLock.cpp: implementation of the CMOOSLock class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOS/libMOOS/Utils/MOOSLock.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"


#ifndef _WIN32
    #include <pthread.h>
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSLock::CMOOSLock(bool bInitial)
{
  UNUSED_PARAMETER(bInitial);
#ifdef _WIN32
    m_hLock = ::CreateEvent(NULL,false,bInitial,NULL);
#else
    pthread_mutex_init(&m_hLock,NULL);
#endif
}

CMOOSLock::~CMOOSLock()
{
#ifdef _WIN32
    if (m_hLock != INVALID_HANDLE_VALUE && m_hLock != 0)
    {
        ::CloseHandle(m_hLock);
    }
#else
      pthread_mutex_destroy( &m_hLock);
#endif


}

void CMOOSLock::Lock()
{
#ifdef _WIN32
    if (m_hLock != INVALID_HANDLE_VALUE && m_hLock != 0)
    {
        ::WaitForSingleObject(m_hLock,INFINITE);
    }
#else

    pthread_mutex_lock( & m_hLock);

#endif

}

void CMOOSLock::UnLock()
{
#ifdef _WIN32
    if (m_hLock != INVALID_HANDLE_VALUE && m_hLock != 0)
    {
        ::SetEvent(m_hLock);
    }
#else
    pthread_mutex_unlock( & m_hLock);
#endif
}
