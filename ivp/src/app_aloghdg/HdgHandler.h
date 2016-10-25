/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HdgHandler.h                                         */
/*    DATE: Oct 24th, 2016                                       */
/*****************************************************************/

#ifndef HEADING_HANDLER_HEADER
#define HEADING_HANDLER_HEADER

#include <string>
#include "LogPlot.h"

class HdgHandler
{
 public:
  HdgHandler();
  ~HdgHandler() {}
  
  bool setALogFileIn(std::string);
  bool setKLogFileOut(std::string);
  void setVerbose(bool v)  {m_verbose=v;}

  bool processALogFile();
  bool processHeadingDeltas();
  bool writeKLogFile();

  bool filterOutSpdLowerThan(double);

  bool writeReport();
  
 protected:  // config parameters

  bool        m_verbose;
  std::string m_alog_file;
  std::string m_klog_file;

 protected: // State variables

  LogPlot  m_lplot_x;
  LogPlot  m_lplot_y;
  LogPlot  m_lplot_hdg;
  LogPlot  m_lplot_spd;

  LogPlot  m_lplot_delta;
};

#endif
