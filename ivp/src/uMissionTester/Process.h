/*****************************************************************************
 *
 ****************************************************************************/

#ifndef _WIN32
#include <stdbool.h>
#endif

#include <string>
#include <list>
#include <stdarg.h>

#ifndef _PROCESS_H
#define _PROCESS_H

class Process{

public:
    Process();
    virtual ~Process();


	virtual bool setCommand(char* app_name, ...);
    virtual bool start(void) = 0;
    virtual bool stop(void) = 0;
    virtual bool isAlive(void) = 0;
protected:
	std::list<std::string> m_args;
};

#endif //_PROCESS_H

/*****************************************************************************
 *
 ****************************************************************************/

