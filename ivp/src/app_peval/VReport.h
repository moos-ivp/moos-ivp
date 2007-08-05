/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VReport.h                                            */
/*    DATE: March 4th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef VREPORT_HEADER
#define VREPORT_HEADER

#include <vector>
#include "SlotReport.h"

class VReport {
  
public:
  VReport(int vehicles=0);
  ~VReport() {};
  
  void addDistInfo(int ix, double time, double dist);
  void addStatInfo(int ix, double time, double dist);

  int  getSize()  {return(prox_report.size());};

  SlotReport getProxReport(int);
  SlotReport getStatReport(int);

protected:
  // PROXIMITY report for each vehicle
  std::vector<SlotReport>  prox_report;
  // ON-STATION report for each vehicle
  std::vector<SlotReport>  stat_report;

};
#endif





