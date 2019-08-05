/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvdColregsV19.h                                  */
/*    DATE: May 16th, 2013 (combined from individual colrg bvs)  */
/*    DATE: Oct 12th, 2018 (V19 from V17, plus refineries)       */
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
 
#ifndef BHV_AVD_COLREGS_V19_HEADER
#define BHV_AVD_COLREGS_V19_HEADER

#include <list>
#include <string>
#include "IvPContactBehavior.h"
#include "XYSegList.h"

class IvPDomain;
class BHV_AvdColregsV19 : public IvPContactBehavior {
 public:
  BHV_AvdColregsV19(IvPDomain);
  ~BHV_AvdColregsV19() {};
  
  void         onHelmStart();
  bool         onRunStatePrior();
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();
  void         onCompleteState();
  std::string  getInfo(std::string);
  double       getDoubleInfo(std::string);

 protected:
  void         updateAvoidMode();

  void         checkModeOvertaking();            // Rule-13
  void         checkModeHeadOn();                // Rule-14
  void         checkModeGiveWay();               // Rule-16
  void         checkModeStandOn();               // Rule-17
  void         checkModeStandOnOT();             // Rule-17 
  void         checkModeCPA();                   // Catch-all
  IvPFunction *buildOvertakingIPF();
  IvPFunction *buildHeadOnIPF();
  IvPFunction *buildGiveWayIPF();
  IvPFunction *buildStandOnIPF();
  IvPFunction *buildCPA_IPF();

  bool         resetAvoidModes(std::string m="none", std::string s="none");
  double       getRelevance() const;
  bool         findDecelerateSpeedRange(double&, double&);
  bool         findAccelerateSpeedRange(double &, double&);

  void         addHeading(double, double);
  void         addRelBng(double, double);
  void         clearHeadings();
  void         clearRelBngs();
  bool         getHeadingRate(double&, double min_secs=4);
  bool         getRelBngRate(double&);

  void         postStatusInfo();

 private: // Configuration Parameters
  double       m_pwt_outer_dist;
  double       m_pwt_inner_dist;
  double       m_min_util_cpa_dist;
  double       m_max_util_cpa_dist;
  double       m_completed_dist;

  double       m_giveway_bow_dist;
  
  double       m_overtaking_bng_range;
  double       m_headon_abs_relbng_thresh;

  std::string  m_pwt_grade;
  std::string  m_contact_type_required;

 private:  // State Variables
  std::string  m_avoid_mode;
  double       m_avoid_mode_elaps_time;
  double       m_avoid_mode_start_time;

  std::string  m_avoid_submode;
  double       m_avoid_submode_elaps_time;
  double       m_avoid_submode_start_time;

  double       m_osh_orig_standon;
  double       m_spd_orig_standon;
  bool         m_standon_set;

  double       m_turn_radius;
  
  std::string  m_debug1;
  std::string  m_debug2;
  std::string  m_debug3;
  std::string  m_debug4;

  bool         m_check_plateaus;
  bool         m_check_validity;
  double       m_pcheck_thresh;
  bool         m_use_refinery;
  
  unsigned int m_iterations;
  bool         m_cn_crossed_os_port_star;

  double       m_actual_pwt;
  double       m_initial_speed;
  XYSegList    m_bearing_segl;

  std::list<double> m_cn_heading_val;
  std::list<double> m_cn_rel_bng_val;
  std::list<double> m_cn_heading_time;
  std::list<double> m_cn_rel_bng_time;
  double            m_memory_time;

  bool m_verbose;
  
  bool   m_no_alert_request;
};

#endif




