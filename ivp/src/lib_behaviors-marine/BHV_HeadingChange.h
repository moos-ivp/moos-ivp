/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HeadingChange.h                                  */
/*    DATE: Feb 11th 2009                                        */
/*****************************************************************/
 
#ifndef BHV_HEADING_CHANGE_HEADER
#define BHV_HEADING_CHANGE_HEADER

#include "IvPBehavior.h"

class BHV_HeadingChange : public IvPBehavior {
public:
  BHV_HeadingChange(IvPDomain);
  ~BHV_HeadingChange() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  void         postTrend();
  bool         checkForTurnCompletion();

 protected: // Configuration parameters
  double       m_heading_delta;
  double       m_turn_speed;
  std::string  m_turn_type;

 protected: // state variables
  double       m_prev_heading;
  double       m_start_heading;
  double       m_start_xpos;
  double       m_start_ypos;
  double       m_start_des_speed;
  double       m_heading_goal;
  std::string  m_state;
  bool         m_sign_positive;

  double       m_curr_time;
  double       m_curr_heading;
  double       m_curr_speed;
  double       m_curr_des_speed;
  double       m_curr_xpos;
  double       m_curr_ypos;

  int          m_completions;

  double       m_turn_time_commenced;

  std::vector<double> m_acc_turn_time;
  std::vector<double> m_acc_heading_delta;
  
  std::vector<double> m_turn_xpos;
  std::vector<double> m_turn_ypos;

};
#endif
