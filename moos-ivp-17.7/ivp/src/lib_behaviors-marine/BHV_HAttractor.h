/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Attractor.h                                      */
/*    DATE: May 10th 2005                                        */
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
 
#ifndef BHV_HATTRACTOR_HEADER
#define BHV_HATTRACTOR_HEADER

#include <string>
#include "IvPBehavior.h"
#include "LinearExtrapolator.h"

class IvPDomain;
class BHV_HAttractor : public IvPBehavior {
public:
  BHV_HAttractor(IvPDomain);
  ~BHV_HAttractor() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();

protected:
  bool   updateInfoIn();  
  double getRelevance(double, double, double, double);
  double getPriority();
  void updateContactList();


private:
  std::string m_contact_name; // Name for them in InfoBuffer
  std::vector<std::string> m_contact_list;

  double  m_min_util_cpa_dist;
  double  m_max_util_cpa_dist;

  double  m_max_priority_range;
  double  m_min_priority_range;

  double  m_giveup_range;
  double  m_patience;

  double  m_time_on_leg;

  double strength;

private: // State Variables
  double  m_osx; // ownship x-position
  double  m_osy; // ownship y-position
  double  m_osh; // ownship heading
  double  m_osv; // ownship velocity

  double  m_cnx; // contact x-position
  double  m_cny; // contact y-position
  double  m_cnh; // contact heading
  double  m_cnv; // contact velocity
  double  m_cnutc; // UTC time of last contact report

  bool    m_extrapolate;
  double  m_decay_start;
  double  m_decay_end;
  double m_cpa_speed;

  LinearExtrapolator m_extrapolator;

};

#endif



















