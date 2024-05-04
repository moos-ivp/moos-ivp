/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT                          */
/*    FILE: BHV_LegRun.h                                         */
/*    DATE: Apr 21st, 2024                                       */
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
 
#ifndef BHV_LEG_RUNZ_HEADER
#define BHV_LEG_RUNZ_HEADER

#include <string>
#include "IvPBehavior.h"
#include "WaypointEngineX.h"
#include "XYPoint.h"
#include "Odometer.h"
#include "WrapDetector.h"
#include "HintHolder.h"
#include "LegRun.h"


class BHV_LegRun : public IvPBehavior {
public:
  BHV_LegRun(IvPDomain);
  ~BHV_LegRun() {}
  
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
  bool   setEngineParam(std::string, std::string);
  bool   setInitLegMode(std::string);
  bool   updateInfoIn();
  void   postStatusReport();

  bool   initTurnPoints();
  
  bool   onRunStateLegMode();
  bool   onRunStateTurnMode();

  void   setMode(std::string);
  void   initLegMode();
  void   handleModeSwitch();
  void   updateLegSpd();
  void   advanceModePending();
  
  IvPFunction* buildOF();

protected: // visuals
  void   postLegSegList(bool active=true);
  void   postLegRing(bool active=true);
  void   postTurnSegList(bool active=true);
  void   postAllSegList(bool active=true);
  void   postSteerPoints(bool active=true);
  void   postLegPoints(bool active=true);
  void   postTurnPreview(bool active=true);
  void   postTurnPoints(bool active=true);
  void   eraseAllViewables();

protected: 
  WaypointEngineX m_wpteng_turn;
  WaypointEngineX m_wpteng_legs;

  LegRun m_legrun;
  
protected: // Evaluation tools
  Odometer     m_odometer;
  WrapDetector m_wrap_detector;
  
protected: // Config vars
  double   m_max_spd;
  double   m_cruise_spd;
  double   m_patience;  // [0,100]
  std::string m_init_leg_mode; // [fixed],close_turn,far_turn
  
  // Event flags unique to this behavior
  std::vector<VarDataPair> m_cycle_flags;
  std::vector<VarDataPair> m_wpt_flags;
  std::vector<VarDataPair> m_leg_flags;
  std::vector<VarDataPair> m_mid_flags;
  std::vector<VarDataPair> m_start_leg_flags;
  std::vector<VarDataPair> m_start_turn_flags;

  double m_mid_pct; 
  
  // Visual hints affecting properties of seglists/points
  HintHolder  m_hints;

  std::vector<double> m_leg_spds; // config var
  int     m_leg_spds_ix;     // state  var
  double  m_leg_spds_curr;   // state  var
  bool    m_leg_spds_repeat; // config var
  bool    m_leg_spds_onturn; // config var

protected: // State vars
  std::string  m_mode;
  std::string  m_mode_pending;
  XYPoint      m_nextpt;
  XYPoint      m_trackpt;
  XYPoint      m_turn_pt1;
  XYPoint      m_turn_pt2;
  XYPoint      m_prev_dpt;  
  bool         m_mid_event_yet;
  bool         m_preview_pending;
  unsigned int m_leg_count;
  unsigned int m_leg_count1;
  unsigned int m_leg_count2;
};

#endif
