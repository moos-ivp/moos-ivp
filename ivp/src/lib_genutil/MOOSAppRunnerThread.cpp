/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE:                                                      */
/*    DATE:                                                      */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
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








