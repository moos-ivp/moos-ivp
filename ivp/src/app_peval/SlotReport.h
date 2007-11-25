/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SlotReport.h                                         */
/*    DATE: March 4th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef PROX_REPORT_HEADER
#define PROX_REPORT_HEADER

#include <vector>

class SlotReport {

public:
  SlotReport() {}
  ~SlotReport() {};

  void setSlots(std::vector<double>);
  void addInfo(double dist, double time);
  void clear();

  int    getSize()           {return(slots.size());};
  double getTimeTotal()      {return(time_total);};
  double getTimeOver()       {return(time_over);};
  double getSlotVal(int ix);
  double getSlotTime(int ix);

protected:
  std::vector<double> slots;
  std::vector<double> times;
  
  double time_over;
  double time_total;

};
#endif






