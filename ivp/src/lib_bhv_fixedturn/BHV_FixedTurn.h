/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_FixedTurn.h                                      */
/*    DATE: Jan 2nd 2023                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef BHV_FIXED_TURN_HEADER
#define BHV_FIXED_TURN_HEADER

#include <string>
#include <list>
#include <map>
#include "IvPBehavior.h"
#include "Odometer.h"
#include "HintHolder.h"
#include "FixedTurnSet.h"

class IvPDomain;
class BHV_FixedTurn : public IvPBehavior {
public:
  BHV_FixedTurn(IvPDomain);
  ~BHV_FixedTurn() {}

public: // Virtual functions overloaded
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete() {m_turn_set.print();}
  void         onRunToIdleState();
  void         onIdleToRunState();
  std::string  expandMacros(std::string);
  
protected:
  IvPFunction *buildOF();

  bool updateOSPos(std::string fail_action="err");
  bool updateOSHdg(std::string fail_action="err");
  bool updateOSSpd(std::string fail_action="err");

  void resetState();
  bool handleNewHdg();
  void postTurnCompleteReport();
  void clearTurnVisuals();
  
  bool setState(std::string);
  std::string getState() const {return(m_state);}

  double getCurrTurnSpd() const;
  double getCurrModHdg() const;
  double getCurrFixTurn() const;
  std::string getCurrTurnDir() const;
  double getCurrTimeOut() const; 
  
 protected: // State vars
  std::string m_state;
  Odometer m_odometer;
  double   m_hdg_delta_sofar;
  double   m_osh_prev;
  double   m_stem_hdg;
  double   m_stem_spd;
  double   m_stem_utc;
  double   m_curr_rudder;
  
  std::vector<XYPoint> m_mark_pts;
  std::list<double>    m_mark_rudder;

  std::set<std::string> m_set_radial_segls;
  
 private: // Config params
  double   m_fix_turn;   // Total delta hdg targeted
  double   m_mod_hdg;    // Delta hdg requested in turning
  bool     m_port_turn;
  double   m_cruise_spd;
  double   m_timeout;
  
  double   m_stale_nav_thresh;

  std::string m_radius_rep_var;

  FixedTurnSet m_turn_set;
  
  // Visual hints affecting properties of seglists/points
  HintHolder m_hints;
};

#endif
