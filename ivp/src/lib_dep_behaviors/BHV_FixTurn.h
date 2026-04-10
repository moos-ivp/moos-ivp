/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_FixTurn.h                                        */
/*    DATE: Oct 16th 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
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
  std::string  isDeprecated();

protected:
  bool handleConfigTurnSpec(std::string);
  
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
  double   m_cruise_speed;

  double   m_stale_nav_thresh;
  bool     m_schedule_repeat;

  std::string m_radius_rep_var;

  bool        m_hide_deprecation_warning;
   
  unsigned int        m_curr_tix;  // Curr Turn Index
  std::vector<double> m_turn_spds;
  std::vector<double> m_turn_hdgs;
  std::vector<double> m_turn_fixh;
  std::vector<std::string> m_turn_dirs;
  
  // Visual hints affecting properties of seglists/points
  HintHolder m_hints;
};

#endif
