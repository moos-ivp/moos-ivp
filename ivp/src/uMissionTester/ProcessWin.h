/*****************************************************************************
 *
 ****************************************************************************/


#ifndef _PROCESSWIN_H
#define _PROCESSWIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif

#include "Process.h"

class ProcessWin : public Process {
#ifdef _WIN32

public:
    ProcessWin();
    virtual ~ProcessWin();

    virtual bool start();
    virtual bool stop();
    virtual bool isAlive();

protected:
    
    STARTUPINFO m_startup_info;
    PROCESS_INFORMATION m_process_info;
    HANDLE m_job;


#endif

}; 




#endif //  _PROCESSWIN_H


/*****************************************************************************
 *
 ****************************************************************************/

