/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_LegRunX.h                                        */
/*    DATE: Oct 2022                                             */
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
 
#ifndef BHV_LEG_RUNX_HEADER
#define BHV_LEG_RUNX_HEADER

#include <string>
#include "IvPBehavior.h"
#include "WaypointEngineX.h"
#include "XYPoint.h"
#include "Odometer.h"
#include "WrapDetector.h"
#include "HintHolder.h"

class BHV_LegRunX : public IvPBehavior {
public:
  BHV_LegRunX(IvPDomain);
  ~BHV_LegRunX() {}
  
  bool        setParam(std::string, std::string);
  IvPFunction* onRunState();
  void        onIdleState();
  void        onRunToIdleState();
  void        onIdleToRunState();
  void        onSetParamComplete();
  void        onCompleteState() {eraseAllViewables();}
  void        postConfigStatus();
  std::string expandMacros(std::string);

protected:
  bool   handleConfigTurnParam(std::string, std::string);
  bool   handleConfigLegSpeed(std::string);
  bool   handleConfigCoord(std::string);
  bool   updateInfoIn();
  void   postStatusReport();

  void   setTurnCoordSpd();
  void   postTurnDist();

  void   postLegSegList(bool active=true);
  void   postTurnSegList(bool active=true);
  void   postAllSegList(bool active=true);
  void   postSteerPoints(bool active=true);
  void   postLegPoints(bool active=true);
  void   postTurnPreview(bool active=true);
  void   postLegSpdsReport();
  void   eraseAllViewables();

  bool      initTurnPoints();
  XYSegList initTurnPoints1(bool preview=false);
  XYSegList initTurnPoints2(bool preview=false);

  bool   onRunStateLegMode();
  bool   onRunStateTurnMode();

  void   clipTurnBiases();
  void   handleModeSwitch();
  void   handleLegLenRequest(double);
  void   handleLegAngRequest(double);
  void   handleShiftPoint();

  IvPFunction* buildOF();
  
protected: 
  WaypointEngineX m_wpteng_turn;
  WaypointEngineX m_wpteng_legs;
  
protected: // Evaluation tools
  Odometer     m_odometer;
  WrapDetector m_wrap_detector;
  
protected: // Config vars
  double   m_cruise_speed;
  double   m_patience;  // [0,100]
  double   m_min_leg_length;
  double   m_min_turn_rad;
  XYPoint  m_vx1;
  XYPoint  m_vx2;
  XYPoint  m_shift_pt;
  bool     m_warn_overshoot;
  double   m_turn_pt_gap;
  
  std::string m_turn1_dir;
  std::string m_turn2_dir;
  double      m_turn1_rad;
  double      m_turn2_rad;
  double      m_turn1_bias;
  double      m_turn2_bias;
  double      m_turn1_ext;
  double      m_turn2_ext;
  
  // Configurable names of MOOS variables for reports
  std::string m_var_report;

  // Event flags unique to this behavior
  std::vector<VarDataPair> m_cycle_flags;
  std::vector<VarDataPair> m_wpt_flags;
  std::vector<VarDataPair> m_leg_flags;
  std::vector<VarDataPair> m_mid_flags;

  double m_mid_pct; 
  
  // Visual hints affecting properties of seglists/points
  HintHolder  m_hints;

  std::vector<double> m_leg_spds;        // config var
  int                 m_leg_spds_ix;     // state  var
  double              m_leg_spds_curr;   // state  var
  bool                m_leg_spds_repeat; // config var
  bool                m_leg_spds_onturn; // config var

  std::string m_coord; 
  
protected: // State vars
  std::string m_mode;
  std::string m_mode_pending;
  
  unsigned int m_leg_count1;
  unsigned int m_leg_count2;
  
  XYPoint  m_nextpt;
  XYPoint  m_trackpt;

  bool     m_mid_event_yet;
  bool     m_new_leg_pending;
  bool     m_preview_pending;
  bool     m_turn_interrupt_pending;

  double   m_leglen_req;
  double   m_leglen_mod_req;

  double   m_legang_req;
  double   m_legang_mod_req;

  // Support for multi-vehicle coordinated turning
  double   m_turn_coord_spd; 
  double   m_turn_dist;

  std::map<std::string, double> m_map_coord_dist;
  std::map<std::string, double> m_map_coord_tstamp;
};

#endif

