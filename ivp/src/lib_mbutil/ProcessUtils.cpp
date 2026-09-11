/*****************************************************************/
/*    NAME: Conlan Cesar                                         */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ProcessUtils.cpp                                     */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*****************************************************************/

#include <cerrno>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ProcessUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: execProcess

static void execProcess(const vector<string>& argv)
{
  vector<char*> cargv;
  for(unsigned int i=0; i<argv.size(); i++)
    cargv.push_back(const_cast<char*>(argv[i].c_str()));
  cargv.push_back(0);

  execvp(cargv[0], &cargv[0]);
  _exit(127);
}

//---------------------------------------------------------
// Procedure: execProcessToFile

static void execProcessToFile(const vector<string>& argv, const string& outfile)
{
  if(!outfile.empty()) {
    int fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0)
      _exit(127);
    if(dup2(fd, STDOUT_FILENO) < 0)
      _exit(127);
    close(fd);
  }

  execProcess(argv);
}

//---------------------------------------------------------
// Procedure: waitForProcess

static bool waitForProcess(pid_t pid)
{
  int status = 0;
  while(waitpid(pid, &status, 0) < 0) {
    if(errno != EINTR)
      return(false);
  }

  return(WIFEXITED(status) && (WEXITSTATUS(status) == 0));
}

//---------------------------------------------------------
// Procedure: runProcess

bool runProcess(const vector<string>& argv, bool detached)
{
  return(runProcessToFile(argv, "", detached));
}

//---------------------------------------------------------
// Procedure: runProcessToFile

bool runProcessToFile(const vector<string>& argv, const string& outfile,
                      bool detached)
{
  if(argv.empty())
    return(false);

  pid_t pid = fork();
  if(pid < 0)
    return(false);

  if(pid == 0) {
    if(!detached)
      execProcessToFile(argv, outfile);

    // The intermediate child exits immediately. Its child is adopted and
    // reaped by the system, so a detached launch cannot leave a zombie behind.
    pid_t grandchild = fork();
    if(grandchild == 0)
      execProcessToFile(argv, outfile);
    _exit(grandchild < 0 ? 127 : 0);
  }

  return(waitForProcess(pid));
}
