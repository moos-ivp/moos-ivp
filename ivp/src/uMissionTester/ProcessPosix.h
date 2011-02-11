/*****************************************************************************
 * AUTHOR:      Christopher Gagner
 * DATE:        June 23, 2010
 * FILE:        ProcessPosix.h
 * DESCRIPTION: TBD
 ****************************************************************************/

#ifndef _PROCESSPOSIX_H
#define	_PROCESSPOSIX_H

#include "Process.h"

#ifndef _WIN32
    #include <stdlib.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <signal.h>
#endif

class ProcessPosix : public Process{
#ifndef _WIN32
public:
    ProcessPosix();
    virtual ~ProcessPosix();

    virtual bool start();
    virtual bool stop();
    virtual bool isAlive();


protected:
    pid_t m_pid;

private:


#endif // NOT _WIN32
}; // END of ProcessPosix class

#endif	/* _PROCESSPOSIX_H */

/*****************************************************************************
 * END of ProcessPosix.h
 ****************************************************************************/
