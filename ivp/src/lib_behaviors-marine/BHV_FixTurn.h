/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_FixTurn.h                                        */
/*    DATE: Oct 16th 2022                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef BHV_FIX_TURN_HEADER
#define BHV_FIX_TURN_HEADER

#include <string>
#include <list>
#include <map>
#include "IvPBehavior.h"
#include "Odometer.h"
#include "HintHolder.h"

class IvPDomain;
class BHV_FixTurn : public IvPBehavior {
public:
  BHV_FixTurn(IvPDomain);
  ~BHV_FixTurn() {}

public: // Virtual functions overloaded
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete() {};
  void         onRunToIdleState();
  void         onIdleToRunState();
  std::string  expandMacros(std::string);
  
protected:
  bool handleConfigTurnSpec(std::string);
  
  IvPFunction *buildOF();

  bool updateOSPos(std::string fail_action="err");
  bool updateOSHdg(std::string fail_action="err");
  bool updateOSSpd(std::string fail_action="err");

  void resetState();
  bool handleNewHdg();

  bool setState(std::string);
  std::string getState() const {return(m_state);}

 protected: // State vars
  std::string m_state;
  Odometer m_odometer;
  double   m_stem_hdg;
  double   m_stem_spd;
  double   m_stem_utc;
  double   m_hdg_delta_sofar;
  double   m_osh_prev;
  
 private: // Config params
  double   m_fix_turn;   // Total delta hdg targeted
  double   m_mod_hdg;    // Delta hdg requested in turning
  bool     m_port_turn;
  double   m_cruise_speed;
  
  double   m_stale_nav_thresh;

  unsigned int        m_curr_tix;  // Curr Turn Index
  std::vector<double> m_turn_hdgs;
  std::vector<double> m_turn_spds;
  
  // Visual hints affecting properties of seglists/points
  HintHolder m_hints;
};

#endif
