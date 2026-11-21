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
//   This file is part of a  MOOS Utility Component. 
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
// uPlayBack.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "uPlayBack.h"
#include "uPlayBackDlg.h"
//#include <initguid.h>
#include "Splash.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUPlayBackApp

BEGIN_MESSAGE_MAP(CUPlayBackApp, CWinApp)
    //{{AFX_MSG_MAP(CUPlayBackApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUPlayBackApp construction

CUPlayBackApp::CUPlayBackApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUPlayBackApp object

CUPlayBackApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUPlayBackApp initialization

BOOL CUPlayBackApp::InitInstance()
{

    AfxEnableControlContainer();

    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
  

    if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
    {
        return TRUE;
    }



    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();            // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();    // Call this when linking to MFC statically
#endif

    CUPlayBackDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

    
CUPlayBackModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

LONG CUPlayBackModule::Unlock()
{
    AfxOleUnlockApp();
    return 0;
}

LONG CUPlayBackModule::Lock()
{
    AfxOleLockApp();
    return 1;
}
LPCTSTR CUPlayBackModule::FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
    while (*p1 != NULL)
    {
        LPCTSTR p = p2;
        while (*p != NULL)
        {
            if (*p1 == *p)
                return CharNext(p1);
            p = CharNext(p);
        }
        p1++;
    }
    return NULL;
}


int CUPlayBackApp::ExitInstance()
{

    return CWinApp::ExitInstance();

}

