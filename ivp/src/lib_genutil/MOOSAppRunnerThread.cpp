#include "MOOSAppRunnerThread.h"

//==============================================================================

MOOSAppRunnerThread::MOOSAppRunnerThread(CMOOSApp *app, 
					 const char *name, 
					 const char *mission_file,
					 int argc, 
					 char **argv)
   : m_name(name), m_mission_file(mission_file)
{
  this->m_app = app;
  
  m_argc = argc;
  m_argv = argv;

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
  int    argc = params->m_argc;
  char **argv = params->m_argv;

  MOOSTrace("starting %s thread\n", name);
  if((argc==0) || (argv==0))
    app->Run(name, mission_file);
  else
    app->Run(name, mission_file, argc, argv);
  
  // As far as I know, this return value is meaningless to both MOOSThread and
  // this app. -CJC   
  return true;
}

//==============================================================================
