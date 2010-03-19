/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: HelmReporter.h                                       */
/*    DATE: March 10th, 2010                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ALOG_HELM_REPORTER_HEADER
#define ALOG_HELM_REPORTER_HEADER

#include <vector>
#include <string>
#include <set>
#include "LifeEventHistory.h"

class HelmReporter
{
 public:
  HelmReporter() {m_report_life=false;};
  ~HelmReporter() {};

  bool handle(const std::string&);
  void printReport();
  
  bool setReportType(std::string);

  void setColorActive(bool v) {m_life_events.setColorActive(v);};
  
 protected:
  LifeEventHistory m_life_events;
  bool             m_report_life;
};

#endif
