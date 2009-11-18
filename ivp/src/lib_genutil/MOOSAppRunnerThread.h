#ifndef IVP_THREAD_UTIL_H
#define IVP_THREAD_UTIL_H

#include "MOOSApp.h"
#include "MOOSThread.h"
#include <string>

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
    MOOSAppRunnerThread(CMOOSApp *app, const char *name, const char *mission_file);

    /**
    At present this is no-op.
    */
    virtual ~MOOSAppRunnerThread();

    /// Gracefully terminates the app's Run() method invocation.  This method
    /// doesn't return until after Run() has returned.
    void quit();
    
    /**
    Blocks until the thread function has returned.
    */
//     void join();

    /// Terminates the thread owned by this object.  Doesn't return until the
    /// thread has definitely ceased execution.
//     void cancel();
    
  private:
    CMOOSApp * m_app;
    const std::string m_name;
    const std::string m_mission_file;
    CMOOSThread * m_thread;
    static bool thread_func(void *pThreadData);
};

#endif
