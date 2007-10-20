/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_SteadyHeading.h                                  */
/*    DATE: July 21st 2005                                       */
/*****************************************************************/
 
#ifndef BHV_STEADY_HEADING_HEADER
#define BHV_STEADY_HEADING_HEADER

#include <list>
#include "IvPBehavior.h"

class BHV_SteadyHeading : public IvPBehavior {
public:
  BHV_SteadyHeading(IvPDomain);
  ~BHV_SteadyHeading() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  bool         updateCurrHeadingTimeLog();
  void         addHeading(double, double);
  bool         getHeadingAvg(double&);

protected:
  std::list<double> heading_val;
  std::list<double> heading_time;
  double            memory_time;
  double            variable_pwt;

  double            summit_delta;
  double            peak_width;
  double            grace_time;
  double            curr_heading;
  double            curr_time;
};
#endif








