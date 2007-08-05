/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_TimeOut.h                                        */
/*    DATE: July 27th 2007 (pulled from BHV_OpRegion)            */
/*****************************************************************/
 
#ifndef BHV_TIME_OUT_HEADER
#define BHV_TIME_OUT_HEADER

#include "IvPBehavior.h"

class BHV_TimeOut : public IvPBehavior {
 public:
  BHV_TimeOut(IvPDomain);
  ~BHV_TimeOut() {};
  
  bool         setParam(std::string, std::string);
  IvPFunction* produceOF();

 protected:
  void      setTimeStamps();

protected: // Configuration Parameters (time in seconds)
  double    m_max_time;

protected: // State Variables (all time values in seconds)
  double    m_previous_time;  
  double    m_current_time;
  double    m_delta_time;
  double    m_start_time;
  double    m_elapsed_time;
  bool      m_first_time;
};
#endif









