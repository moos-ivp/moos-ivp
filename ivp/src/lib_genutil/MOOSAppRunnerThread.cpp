#include "MOOSAppRunnerThread.h"

#include "MOOSGenLibGlobalHelper.h"

//==============================================================================

MOOSAppRunnerThread::MOOSAppRunnerThread(CMOOSApp *app, char *name, char *mission_file)
{
  this->m_pApp = app;
  this->m_name = name;
  this->m_mission_file = mission_file;

  void * pMoosThreadParams = static_cast<void*>(this);

  this->m_thread = new CMOOSThread(thread_func, pMoosThreadParams);
  if (this->m_thread->Start()) {
    MOOSTrace("%s thread spawned\n", m_name);
  }
  else {
    MOOSTrace("failed to start %s thread\n", m_name);
  }
}

//==============================================================================

MOOSAppRunnerThread::~MOOSAppRunnerThread()
{
}

//==============================================================================

void MOOSAppRunnerThread::join() 
{
  this->m_thread->Stop();
}

//==============================================================================

bool MOOSAppRunnerThread::thread_func(void *pThreadData)
{
  MOOSAppRunnerThread *params = 
    reinterpret_cast<MOOSAppRunnerThread*>(pThreadData);

  CMOOSApp *app      = params->m_pApp;
  char *name         = params->m_name;
  char *mission_file = params->m_mission_file;
  
  MOOSTrace("starting %s thread\n", name);
  app->Run(name, mission_file);
  
  // As far as I know, this return value is meaningless to both MOOSThread and
  // this app. -CJC   
  return true;
}

//==============================================================================
