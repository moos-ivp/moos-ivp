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

#include "XPCProcess.h"
#include "XPCProcessAttrib.h"
#include "XPCException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XPCProcess::XPCProcess(XPCProcessAttrib& attrib):
                    procAttrib (attrib)
{

if (!CreateProcess (attrib.pApplicationName, 
                attrib.pCommandLine, 
                attrib.pProcessSA, 
                attrib.pThreadSA, 
                attrib.bInheritHandles, 
                attrib.dwCreationFlags, 
                attrib.pEnvironment, 
                attrib.pCurrentDirectory, 
                attrib.pSI, 
                attrib.pProcessInfo))
    {
        XPCException ex (ErrorString("CreateProcess failed"));
        throw ex;
        return;
    }
}

XPCProcess::~XPCProcess()
{

}

char * XPCProcess::ErrorString(char * sLeader)
{
    static char buf[255];
    LPVOID lpMsgBuf;
    UINT ErrNo;

    FormatMessage (
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            ErrNo=GetLastError (),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL);

    wsprintf (buf, "%s: %s", sLeader, (LPTSTR)lpMsgBuf);

    LocalFree (lpMsgBuf);
    return buf;
}

DWORD XPCProcess::dwGetExitCode()
{
    DWORD dwRet=0;

    if (!GetExitCodeProcess (procAttrib.pGetProcessInformation()->hProcess, &dwRet))
    {
        XPCException ex (ErrorString("GetExitCodeProcess failed"));
        throw ex;
    }
    return (dwRet);
}


void XPCProcess::vTerminate()
{
    if (!TerminateProcess (procAttrib.pGetProcessInformation()->hProcess, 1))
    {
        XPCException ex (ErrorString("TerminateProcess failed"));
        throw ex;
        return;
    }
}

void XPCProcess::vWaitForTerminate(DWORD dwWaitTime)
{
    if (WaitForSingleObject (procAttrib.pGetProcessInformation()->hProcess,
            dwWaitTime) == WAIT_FAILED)
    {
        XPCException ex (ErrorString("WaitForSingleObject failed"));
        throw ex;
        return;
    }
}

void XPCProcess::vResume()
{
    if (ResumeThread (procAttrib.pGetProcessInformation()->hThread) == 0xFFFFFFFF)
    {
        XPCException ex (ErrorString("ResumeThread failed"));
        throw ex;
        return;
    }
}
