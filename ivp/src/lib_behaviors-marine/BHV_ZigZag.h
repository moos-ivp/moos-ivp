/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_ZigZag.h                                         */
/*    DATE: Sep 16th 2022                                        */
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

#ifndef BHV_ZIGZAG_HEADER
#define BHV_ZIGZAG_HEADER

#include <string>
#include <list>
#include <map>
#include "IvPBehavior.h"
#include "Odometer.h"

class IvPDomain;
class BHV_ZigZag : public IvPBehavior {
public:
  BHV_ZigZag(IvPDomain);
  ~BHV_ZigZag() {}

public: // Virtual functions overloaded
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete() {};
  void         onRunToIdleState();
  void         onIdleToRunState();
  std::string  expandMacros(std::string);
  
protected:
  IvPFunction *buildOF();

  bool handleConfigZigFirst(std::string);
  bool handleConfigZigZags(std::string);
  bool handleConfigVisualHint(std::string);
  bool handleConfigZigAngle(std::string);
  bool handleConfigZigAngleFierce(std::string);
  bool handleConfigSpeed(std::string);
  bool handleConfigModSpeed(std::string);
  
  bool updateOSPos(std::string fail_action="err");
  bool updateOSHdg(std::string fail_action="err");
  bool updateOSSpd(std::string fail_action="err");
  void updateOdometry();
  void resetOdometry();

  void updateSetHdg();
  void updateReqHdg();
  void setState(std::string);
  std::string getState() const {return(m_state);}
  
 protected: // visuals
  void postSetHdgLine();
  void postReqHdgLine();
  void eraseSetHdgLine();
  void eraseReqHdgLine();

  bool legsComplete() const {return(m_zig_cnt > m_max_zig_legs);}  
  
 protected: // State vars
  Odometer m_odometer;
  Odometer m_zig_odo;
  Odometer m_zag_odo;

  double   m_req_hdg;
  double   m_set_hdg;

  std::string m_state;  // stem, port, star

  double m_stem_x1;
  double m_stem_y1;
  double m_stem_x2;
  double m_stem_y2;
  double m_stem_dist;
  double m_stem_odo;
  
  double m_zig_spd_start;
  double m_zig_spd_min;
  double m_zig_spd_delta;
  
  unsigned int m_zig_cnt;
  unsigned int m_zig_cnt_ever;
  
 private: // Config params
  double m_hdg_thresh;
  double m_zig_angle;
  double m_zig_angle_fierce;
  
  double m_stem_hdg;
  double m_speed;
  double m_speed_orig;
  bool   m_speed_on_active;
  
  double m_stale_nav_thresh;

  bool   m_stem_on_active;
  bool   m_fierce_zigging;
  
  bool   m_draw_set_hdg;
  bool   m_draw_req_hdg;

  unsigned int m_max_zig_legs;
  double       m_max_stem_odo;
  
  std::string m_hint_set_hdg_color;
  std::string m_hint_req_hdg_color;
  
  std::string m_zig_first;

  std::vector<VarDataPair> m_zig_flags;
  std::vector<VarDataPair> m_zag_flags;
  std::vector<VarDataPair> m_port_flags;
  std::vector<VarDataPair> m_star_flags;
  std::vector<VarDataPair> m_portx_flags;
  std::vector<VarDataPair> m_starx_flags;
};

#endif

