/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_HTrail.h                                          */
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
 
#ifndef BHV_HTRAIL_HEADER
#define BHV_HTRAIL_HEADER

#include "IvPBehavior.h"
#include "LinearExtrapolator.h"

using namespace std;

class IvPDomain;
class BHV_HTrail : public IvPBehavior {
public:
  BHV_HTrail(IvPDomain);
  ~BHV_HTrail() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  bool   updateInfoIn();  
  double getRelevance();
  double getPriority();
  int decode(string);
  
private: // Configuration parameters
  std::string m_contact; 
  double  m_trail_range;
  double  m_trail_angle;
  double  m_radius;
  double  m_max_range;
  double  m_max_util_cpa_dist;
  double  m_min_util_cpa_dist;

private: // State Variables
  double  m_osx; // ownship x-position
  double  m_osy; // ownship y-position
  double  m_osh; // ownship heading
  double  m_osv; // ownship velocity

  double  m_cnx; // contact x-position
  double  m_cny; // contact y-position
  double  m_cnh; // contact heading
  double  m_cnv; // contact velocity
  double  m_cnt; // contact time

  bool    m_interpolate;
 
  double contact_x,contact_y,contact_heading,contact_speed,contact_time;
  double m_obsolete,m_speed_delta,curr_time;
  std::string them_name; // Name for them in InfoBuffer
  int them_id,contact_id,new_state,state;
  bool my_contact ;

  LinearExtrapolator m_extrapolator;

};
#endif










