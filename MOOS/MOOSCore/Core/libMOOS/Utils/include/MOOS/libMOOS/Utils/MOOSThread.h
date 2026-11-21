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

#ifndef MOOSThreadh
#define MOOSThreadh

#include "MOOS/libMOOS/Utils/MOOSLock.h"
#include "MOOS/libMOOS/Utils/MOOSScopedLock.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h"
#include <iostream>
#ifndef _WIN32
#include <errno.h>
#endif

//! Implements a cross platform thread*/
class CMOOSThread
{

#ifdef _WIN32
    typedef unsigned long thread_id;
#else
    typedef  pthread_t thread_id;
#endif


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
        
        Verbose(false);

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
    	if(IsThreadRunning())
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
    
    
    inline void RequestQuit(){
        SetQuitFlag(true);
    }
    
    inline bool IsQuitRequested()
    {
        return GetQuitFlag();
    }

    

    inline bool IsThreadRunning()
    {
        return GetRunningFlag();
    }


    inline void Name(const std::string & sName)
    {
    	m_sName = sName;
    }

    inline std::string  Name()
    {
    	return m_sName;
    }


    //! Starts the thread running (as long as the class has been properly initialised!)
    bool Start(bool bCreatUnixDetached = false)
    {
#ifdef _WIN32
		bCreatUnixDetached;
#endif
		if(IsThreadRunning())
		    return false;

		m_bQuitRequested = false;

		SetRunningFlag(true);
 
#ifdef _WIN32
        m_hThread = ::CreateThread(NULL,
            0,
            CallbackProc,
            this,
            CREATE_SUSPENDED,
            &m_nThreadID);
        ResumeThread(m_hThread);
#else
        pthread_attr_t attr;
        pthread_attr_init(&attr);
		if(bCreatUnixDetached)
		{
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		}
        int nStatus = pthread_create( &m_nThreadID,&attr,CallbackProc,this);
        if(nStatus!=0) {
            SetRunningFlag(false);
            return false;
        }
#endif


        if(!Name().empty() && m_bVerbose)
        	std::cerr<<"Thread "<<Name()<<" started\n";


 
        return true;
    }
    
    //Get Native Thread Handle
    thread_id GetNativeThreadHandle(){
        return m_nThreadID;
    }
    
    
    // Requests for the running thread to quit, and sleeps until
    // it does.
    // This method does NOT actively stop the thread, it simply
    // makes a request to quit.  This relies on the running
    // thread occasionally polling the IsQuitRequested() method
    // of this class, to find out whether it needs to stop itself.
    bool Stop()
    {

    	if(!IsThreadRunning())
    		return true;

    	SetQuitFlag(true);
        
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
        int retval = pthread_join( m_nThreadID,&Result);
        if (retval != 0)
        {
			switch (retval)
			{
				case EINVAL:
					MOOSTrace("pthread_join returned error: EINVAL\n", retval);
					break;
				case ESRCH:
					MOOSTrace("pthread_join returned error: ESRCH\n", retval);
					break;
				case EDEADLK:
					MOOSTrace("pthread_join returned error: EDEADLK\n", retval);
					break;
			}
			
            MOOSTrace("pthread_join returned error: %d\n", retval);
        }
        
#endif

        

        SetRunningFlag(false);

        if(!Name().empty() && m_bVerbose)
        	std::cerr<<"Thread "<<Name()<<" stopped\n";


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
        MOOS::ScopedLock lock(m_APILock);
        m_bQuitRequested = bState;
    }
    
    void SetRunningFlag(bool bState) {
        MOOS::ScopedLock lock(m_APILock);
        m_bRunning = bState;
    }
    
    void Verbose(bool bV){
    	m_bVerbose = bV;
    }

    bool GetQuitFlag() {
        MOOS::ScopedLock lock(m_APILock);
        return  m_bQuitRequested;
    }
    
    bool GetRunningFlag() {
        MOOS::ScopedLock lock(m_APILock);
        return  m_bRunning;
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
    CMOOSLock m_APILock;


    
#ifdef _WIN32
    HANDLE m_hThread;
    thread_id  m_nThreadID;
#endif
	
#ifndef _WIN32
    thread_id m_nThreadID;
#endif

    ////////////////
    // These are only accessed through Set/Get
	// volatile to give thread safety of a basic type
    volatile bool m_bRunning;
    volatile bool m_bQuitRequested;

    bool m_bVerbose;

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
    
    std::string m_sName;





};
#endif


