/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Launcher.h                                           */
/*    DATE: March 6th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef PEVAL_LAUNCHER_HEADER
#define PEVAL_LAUNCHER_HEADER

#include "NavPlot.h"
#include "LogPlot.h"

class Launcher {
public:
  Launcher() {};
  ~Launcher() {};

  void handleParam(std::string);
  bool launch();
  
protected:
  bool checkParams();
  void handleSkews();
  void handleNavPlots();
  void handleLogPlots();

protected:
  std::vector<std::string> slog_files;
  std::vector<double>      slog_files_skew;
  std::vector<std::string> slog_files_vehiname;

  std::vector<NavPlot>     navplots;
  std::vector<LogPlot>     logplots;
};
#endif






