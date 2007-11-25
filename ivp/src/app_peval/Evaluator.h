/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Evaluator.h                                          */
/*    DATE: March 4th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef NAVPLOT_EVALUATOR_HEADER
#define NAVPLOT_EVALUATOR_HEADER

#include <vector>
#include "NavPlot.h"
#include "LogPlot.h"
#include "VReport.h"

class Evaluator {

public:
  Evaluator() {};
  ~Evaluator() {};

  void addNavPlot(NavPlot plot) {navplots.push_back(plot);};
  void addLogPlot(LogPlot plot) {logplots.push_back(plot);};
  void evaluate(double step_time);
  void makeLatexTable(std::string);

protected:
  double calcVehicleDist(int ix1, int ix2, double time);


protected:  // Input
  std::vector<NavPlot> navplots;
  std::vector<LogPlot> logplots;

protected:  // Output
  VReport vreport;

};
#endif






