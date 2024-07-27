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
//   This file is part of a  MOOS CORE Component. 
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
//   The XPC classes in MOOS are modified versions of the source provided 
//   in "Making UNIX and Windows NT Talk" by Mark Nadelson and Thomas Haga 
//
//////////////////////////    END_GPL    //////////////////////////////////

#if !defined(XPCPROCESSATTRIB_H_)
#define XPCPROCESSATTRIB_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include <windows.h>
#include "XPCException.h"

class XPCProcessAttrib  
{
    friend class XPCProcess;

public:
    XPCProcessAttrib(LPTSTR appName= NULL,
                    LPSTR commandLine= NULL,
                    BOOL processInheritable= FALSE,
                    BOOL threadInheritable= FALSE,
                    BOOL inheritHandles= FALSE,
                    DWORD creationFlags= 0,
                    LPVOID environment= NULL,
                    LPTSTR currentDirectory= NULL);

    virtual ~XPCProcessAttrib();

    void AddEnvironmentVariable (LPCTSTR param, LPCTSTR val);

    LPVOID GetEnvironmentBlock()
    {return pEnvironment;}

    void vSetProcessInheritable (BOOL bInherit) 
    {pProcessSA->bInheritHandle= bInherit;}

    void vSetThreadInheritable (BOOL bInherit)
    {pThreadSA->bInheritHandle= bInherit;}

    void vSetInheritHandles (BOOL bInherit)
    {bInheritHandles= bInherit;}

    void vSetCreationFlag (DWORD dwBitmap)
    {dwCreationFlags |= dwBitmap;}

    void vSetEnvironment (LPVOID pEnv);
    void vSetCommandLine (LPTSTR pCmd);
    void vSetApplicationName (LPTSTR pApp);
    void vSetCurrentDirectory (LPTSTR pDir);

    void vSetStartupInfo (LPSTARTUPINFO pStrt)
    {memmove (pSI, pStrt, sizeof (STARTUPINFO));}

    void vGetProcessInformation (LPPROCESS_INFORMATION pPI)
    {memmove (pPI, pProcessInfo, sizeof (PROCESS_INFORMATION));}

    LPPROCESS_INFORMATION pGetProcessInformation()
    {return pProcessInfo;}
    
private:
    DWORD dwEnvSize;
    LPTSTR pCurrentDirectory;
    LPVOID pEnvironment;
    DWORD dwCreationFlags;
    BOOL bInheritHandles;
    LPTSTR pCommandLine;
    LPTSTR pApplicationName;
    LPPROCESS_INFORMATION pProcessInfo;
    LPSECURITY_ATTRIBUTES pThreadSA;
    LPSECURITY_ATTRIBUTES pProcessSA;
    LPSTARTUPINFO pSI;

    char * ErrorString(char * sLeader);
};

#endif // !defined(XPCPROCESSATTRIB_H_)
