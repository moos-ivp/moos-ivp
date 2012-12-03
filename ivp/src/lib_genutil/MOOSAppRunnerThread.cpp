#include "MOOSAppRunnerThread.h"

//==============================================================================

MOOSAppRunnerThread::MOOSAppRunnerThread(CMOOSApp *app, const char *name, const char *mission_file)
   : m_name(name), m_mission_file(mission_file)
{
   
   
  this->m_app = app;
  
  void * pMoosThreadParams = static_cast<void*>(this);

  this->m_thread = new CMOOSThread(thread_func, pMoosThreadParams);
  if (this->m_thread->Start()) {
    MOOSTrace("%s thread spawned\n", m_name.c_str());
  }
  else {
    MOOSTrace("failed to start %s thread\n", m_name.c_str());
  }
}

//==============================================================================

MOOSAppRunnerThread::~MOOSAppRunnerThread()
{
    delete m_thread;
}

//==============================================================================

void MOOSAppRunnerThread::quit()
{
  m_app->RequestQuit();
  m_thread->Stop(); // This is a blocking call.
}

//==============================================================================

bool MOOSAppRunnerThread::thread_func(void *pThreadData)
{
  MOOSAppRunnerThread *params = 
    reinterpret_cast<MOOSAppRunnerThread*>(pThreadData);

  CMOOSApp *app      = params->m_app;
  const char *name         = params->m_name.c_str();
  const char *mission_file = params->m_mission_file.c_str();
  
  MOOSTrace("starting %s thread\n", name);
  app->Run(name, mission_file);
  
  // As far as I know, this return value is meaningless to both MOOSThread and
  // this app. -CJC   
  return true;
}

//==============================================================================
