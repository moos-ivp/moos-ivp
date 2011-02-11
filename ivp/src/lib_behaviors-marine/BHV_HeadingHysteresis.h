/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HeadingHysteresis.h                              */
/*    DATE: July 30th 2009                                       */
/*****************************************************************/
 
#ifndef BHV_HEADING_HYSTERESIS_HEADER
#define BHV_HEADING_HYSTERESIS_HEADER

#include <list>
#include "IvPBehavior.h"

class BHV_HeadingHysteresis : public IvPBehavior {
 public:
  BHV_HeadingHysteresis(IvPDomain);
  ~BHV_HeadingHysteresis() {};
  
  IvPFunction* onRunState();
  void         onIdleState();
  bool         setParam(std::string, std::string);
  
protected:
  std::string updateHeadingHistory();
  void        addHeadingEntry(double value, double time);
  double      getHeadingAverage();
  double      getHeadingVariance(double);

private: // Configuration Parameters
  double       m_memory_time;
  double       m_peakwidth;
  double       m_basewidth;
  std::string  m_variable_name;

private: // State Variables
  std::list<double> m_heading_val;
  std::list<double> m_heading_time;

  double m_current_heading;
};
#endif










