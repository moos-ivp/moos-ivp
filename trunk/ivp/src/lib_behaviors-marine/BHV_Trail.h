/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Trail.h                                          */
/*    DATE: Jul 3rd 2005 Sunday morning at Brueggers             */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#ifndef BHV_TRAIL_HEADER
#define BHV_TRAIL_HEADER

#include <string>
#include "IvPContactBehavior.h"

class IvPDomain;
class BHV_Trail : public IvPContactBehavior {
public:
  BHV_Trail(IvPDomain);
  ~BHV_Trail() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onRunToIdleState();

protected:
  double  getRelevance();
  double  getPriority();

  void    postViewableTrailPoint();
  void    postErasableTrailPoint();
  
 protected: // State variables

  XYPoint m_trail_point;

private: // Configuration parameters
  double  m_trail_range;
  double  m_trail_angle;
  double  m_radius;
  double  m_nm_radius;
  double  m_max_range;
  double  m_max_util_cpa_dist;
  bool    m_angle_relative;
};
#endif












