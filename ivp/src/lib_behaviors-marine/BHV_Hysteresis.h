/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_Hysteresis.h                                     */
/*    DATE: August 4th 2008 (aboard the Alliance)                */
/*****************************************************************/
 
#ifndef BHV_HYSTERESIS_HEADER
#define BHV_HYSTERESIS_HEADER

#include <list>
#include "IvPBehavior.h"

class BHV_Hysteresis : public IvPBehavior {
public:
  BHV_Hysteresis(IvPDomain);
  ~BHV_Hysteresis() {};
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);

protected:
  std::string updateHeadingAvg();
  void        addHeading(double, double);
  double      getHeadingAverage();
  double      getHeadingVariance(double);


private: // Configuration Parameters

  double m_memory_time;
  double m_turn_range;

private: // State Variables

  std::list<double> m_heading_val;
  std::list<double> m_heading_time;

  double m_current_heading;
  double m_current_speed;

  double m_min_heading_window;
  double m_max_heading_window;
  double m_max_window_utility;

  double m_peakwidth;
  double m_basewidth;
};
#endif










