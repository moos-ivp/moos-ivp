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
                          CMLLock.cpp  -  description
                             -------------------
    begin                : Mon Dec 18 2000
    copyright            : (C) 2000 by pnewman
    email                : pnewman@mit.edu
 ***************************************************************************/


// MOOSLock.cpp: implementation of the CMOOSLock class.
//
//////////////////////////////////////////////////////////////////////

#include "MOOSLock.h"



#ifndef _WIN32
    #include <pthread.h>
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSLock::CMOOSLock(bool bInitial)
{
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
