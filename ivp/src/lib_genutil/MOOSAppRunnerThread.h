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
#ifndef IVP_THREAD_UTIL_H
#define IVP_THREAD_UTIL_H

#include "MOOS/libMOOS/MOOSLib.h"
#include <string>

/**
Creates a thread in which to run the specified CMOOSApp, and starts that thread.
This class doesn't offer any way to cancel, join, or otherwise interact with the
thread once it's created.  

Deleting this class will not stop the thread.
*/
class MOOSAppRunnerThread {
  public:
  /*
     Creates and starts a thread which will execute app->Run(name, mission_file).
     @param app The CMOOSApp whose Run method is to be executed in this thread.
     @param name
     @param mission_file
  */
  MOOSAppRunnerThread(CMOOSApp *app, const char *name, const char *mission_file,
		      int argc=0, char**argv=0);

  virtual ~MOOSAppRunnerThread();
  
  /// Gracefully terminates the app's Run() method invocation.  This method
  /// doesn't return until after Run() has returned.
  void quit();
  bool isRunning(){return m_thread->IsThreadRunning();}
  
  /*
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
  int           m_argc;
  char**        m_argv;


  static bool thread_func(void *pThreadData);
};

#endif








