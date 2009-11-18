/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidCollision.h                                 */
/*    DATE: Nov 18th 2006                                        */
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
 
#ifndef BHV_AVOID_COLLISION_HEADER
#define BHV_AVOID_COLLISION_HEADER

#include "IvPBehavior.h"
#include "LinearExtrapolator.h"

class IvPDomain;
class BHV_AvoidCollision : public IvPBehavior {
public:
  BHV_AvoidCollision(IvPDomain);
  ~BHV_AvoidCollision() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  bool   getBufferInfo();
  double getRelevance();
  double getPriority();
  void   postInfo(double, double);
  void   postRange(bool ok=true);
  
private: // Configuration Parameters

  std::string m_contact; // Name for them in InfoBuffer
  std::string m_active_grade;

  double m_active_outer_dist;
  double m_active_inner_dist;

  double m_collision_dist;
  double m_all_clear_dist;

  double m_roc_max_heighten; // Rate of Closure w/ max relevance heightening
  double m_roc_max_dampen;   // Rate of Closure w/ max relevance dampening

private:  // State Variables

  double m_osx; // Current ownship x position (meters) 
  double m_osy; // Current ownship y position (meters) 
  double m_osh; // Current ownship heading (degrees 0-359)
  double m_osv; // Current ownship speed (meters) 

  double m_cnx; // Current contact x position (meters) 
  double m_cny; // Current contact y position (meters) 
  double m_cnh; // Current contact heading (degrees 0-359)
  double m_cnv; // Current contact speed (meters) 
  double m_cnutc; // UTC time of last contact report

  double m_curr_distance;
  double m_curr_closing_spd;

  bool   m_on_no_contact_ok;  // true if no trouble reported on no contact

  bool    m_extrapolate;
  double  m_decay_start;
  double  m_decay_end;

  LinearExtrapolator m_extrapolator;


};
#endif









