#ifndef IVP_THREAD_UTIL_H
#define IVP_THREAD_UTIL_H

#include "MOOSApp.h"
#include "MOOSThread.h"

/**
Creates a thread in which to run the specified CMOOSApp, and starts that thread.
This class doesn't offer any way to cancel, join, or otherwise interact with the
thread once it's created.  

Deleting this class will not stop the thread.
*/
class MOOSAppRunnerThread {
  public:
    /**
    Creates and starts a thread which will execute app->Run(name, mission_file).
    @param app The CMOOSApp whose Run method is to be executed in this thread.
    @param name
    @param mission_file
    */
    MOOSAppRunnerThread(CMOOSApp *app, char *name, char *mission_file);

    /**
    At present this is no-op.
    */
    virtual ~MOOSAppRunnerThread();

    /**
    Blocks until the thread function has returned.
    */
    void join();
  
  private:
    CMOOSApp * m_pApp;
    char * m_name;
    char * m_mission_file;
    CMOOSThread * m_thread;
    static bool thread_func(void *pThreadData);
};

#endif
