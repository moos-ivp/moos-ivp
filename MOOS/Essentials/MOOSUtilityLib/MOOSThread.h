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

#ifndef MOOSThreadh
#define MOOSThreadh

#include <MOOSGenLib/MOOSLock.h>
#include <MOOSGenLib/MOOSGenLib.h>

//! Implements a cross platform thread*/
class CMOOSThread
{
private:
    //! t_pfnWorkerFunc is a pointer to a thread worker function
    typedef bool (*t_pfnWorkerFunc) (void *pThreadData);
  
public:
    
    CMOOSThread()
    {
        m_bRunning = false;
        m_bQuitRequested = false;
        m_pfnThreadFunc = NULL;
        m_pThreadData = NULL;
        
#ifdef _WIN32
        m_hThread = NULL;
#endif
        m_nThreadID = 0;
        
    }
    
    
    CMOOSThread(t_pfnWorkerFunc pfnThreadFunc, void *pThreadData)
    {
        m_bRunning = false;
        m_bQuitRequested = false;
        m_pfnThreadFunc = NULL;
        m_pThreadData = NULL;
        
#ifdef _WIN32
        m_hThread = NULL;
#endif
        m_nThreadID = 0;
        
        Initialise(pfnThreadFunc, pThreadData);
    }
    

    
    //! Destructor just stops the thread if there's one running
    ~CMOOSThread()
    {
        Stop();
    }
    
    
    
    bool Initialise(t_pfnWorkerFunc pfnThreadFunc, void *pThreadData)
    {
        m_lock.Lock();
        {
            m_pfnThreadFunc  = pfnThreadFunc;
            m_pThreadData    = pThreadData;
            m_bQuitRequested = false;
        }
        m_lock.UnLock();
        
        return (m_pfnThreadFunc != NULL);
    }
    
    
    
    inline bool IsQuitRequested()
    {
        return GetQuitFlag();
    }

    

    inline bool IsThreadRunning()
    {
        return GetRunningFlag();
    }



    //! Starts the thread running (as long as the class has been properly initialised!)
    bool Start()
    {
        m_lock.Lock();
        {
            if (m_bRunning) {
                m_lock.UnLock();
                return false;
            }
            
            m_bRunning       = true;
            m_bQuitRequested = false;
        }
        m_lock.UnLock();
 
#ifdef _WIN32
        m_hThread = ::CreateThread(NULL,
            0,
            CallbackProc,
            this,
            CREATE_SUSPENDED,
            &m_nThreadID);
        ResumeThread(m_hThread);
#else
        int Status = pthread_create( (pthread_t*)&m_nThreadID,NULL,CallbackProc,this);
        if(Status!=0) {
            SetRunningFlag(false);
            return false;
        }
#endif
 
        return true;
    }
    
    
    
    // Requests for the running thread to quit, and sleeps until
    // it does.
    // This method does NOT actively stop the thread, it simply
    // makes a request to quit.  This relies on the running
    // thread occasionally polling the IsQuitRequested() method
    // of this class, to find out whether it needs to stop itself.
    bool Stop()
    {
        m_lock.Lock();
        {
            if (!m_bRunning || m_bQuitRequested) {
                m_lock.UnLock();
                return true;
            }
            m_bQuitRequested = true;
        }
        m_lock.UnLock();
        
        // Now wait for the thread to finish.  It's important that the
        // mutex isn't locked during the wait, because the thread may
        // try to lock the mutex before it gets round to checking the
        // quit flag.
#ifdef _WIN32
        if (m_hThread) {
            WaitForSingleObject(m_hThread,INFINITE);
        }
#else
        void * Result;
        int retval = pthread_join( (pthread_t)m_nThreadID,&Result);
        if (retval != 0)
        {
            MOOSTrace("pthread_join returned error: %d\n", retval);
        }
#endif
        
        // There is the potential for a race condition here, if some other
        // code tries to read and act on the running flag between pthread_join
        // finishing and the running flag being set to false.  Yikes!
        SetRunningFlag(false);

        return true;
    }
    
    
    
public:
    
#ifdef _WIN32
#define TCB DWORD WINAPI
#else
#define TCB void*
#endif
    
    // This is the callback called by the OS when the thread starts.
    // It hands control over to the work method
    static TCB CallbackProc(void *lpThis)
    {
        CMOOSThread* pMe = static_cast<CMOOSThread*> (lpThis);
        
#ifndef _WIN32
        pMe->Work();
	return NULL;	
#else
        return pMe->Work();
#endif
        
    }
    
    
    
private:
    
    void SetQuitFlag(bool bState) {
        m_lock.Lock();
        m_bQuitRequested = bState;
        m_lock.UnLock();
    }
    
    void SetRunningFlag(bool bState) {
        m_lock.Lock();
        m_bRunning = bState;
        m_lock.UnLock();
    }
    
    bool GetQuitFlag() {
        bool bState = false;
        m_lock.Lock();
        bState = m_bQuitRequested;
        m_lock.UnLock();
        return bState;
    }
    
    bool GetRunningFlag() {
        bool bState = false;
        m_lock.Lock();
        bState = m_bRunning;
        m_lock.UnLock();
        return bState;
    }
    

private:

    // This is the method that actually fires off the worker
    // function in the owner object
    bool Work()
    {
        if(m_pfnThreadFunc != NULL)
        {
            SetRunningFlag(true);
            // The next line will block until the thread completes
            bool retval = (*m_pfnThreadFunc)(m_pThreadData);
            SetRunningFlag(false);
            return retval;
        }
        return false;
    }
    
    
private:
    
    CMOOSLock m_lock;
    
#ifdef _WIN32
    HANDLE m_hThread;
#endif
    unsigned long m_nThreadID;

    ////////////////
    // These are only accessed through Set/Get
    // functions with mutexes
    bool m_bRunning;
    bool m_bQuitRequested;
    ////////////////
    
    // This is where we store the address of the function
    // that gets called when the thread fires up
    t_pfnWorkerFunc m_pfnThreadFunc;
    
    // This is normally used for passing the
    // 'this' pointer of the owner class to
    // its static worker method. But it
    // could also be a pointer to a struct
    // containing a load of stuff.  It's up
    // to the owner to decide.
    void *m_pThreadData;
    
};
#endif


