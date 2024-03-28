/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BHV_Trail.h                                          */
/*    DATE: Jul 3rd 2005 Sunday morning at Brueggers             */
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
 
#ifndef BHV_TRAIL_HEADER
#define BHV_TRAIL_HEADER

#include <string>
#include "IvPContactBehavior.h"

class IvPDomain;
class BHV_Trail : public IvPContactBehavior {
public:
  BHV_Trail(IvPDomain);
  ~BHV_Trail() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onRunToIdleState();
  void         onIdleState();
  void         onHelmStart();

protected:
  double  getRelevance();
  double  getPriority();

  bool    handleConfigModTrailRange(double);
  bool    handleConfigModTrailRangePct(double);
  void    postViewableTrailPoint();
  double  updateTrailDistance();  
  void    calculateTrailPoint();

private: // Configuration parameters
  double  m_min_trail_range;
  double  m_trail_range;
  double  m_trail_angle;
  double  m_radius;
  double  m_nm_radius;
  double  m_pwt_outer_dist;
  double  m_max_util_cpa_dist;
  double  m_trail_pt_x;
  double  m_trail_pt_y;
  bool    m_angle_relative;
  bool    m_post_trail_dist_on_idle;

  bool    m_no_alert_request;
};
#endif

