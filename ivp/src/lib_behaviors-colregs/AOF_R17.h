/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_R17.h                                            */
/*    DATE: Feb 9th, 2001 (CPA logic written)                    */
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
 
#ifndef AOF_R17_HEADER
#define AOF_R17_HEADER

#include <string> 
#include "AOF_Contact.h"


class AOF_R17: public AOF_Contact {
public:
  AOF_R17(IvPDomain);
  ~AOF_R17() {};

 public: // virtual functions   
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
  bool   initialize();
  
 protected: // non-virtual functions
  double utilityHold(double hdg, double spd) const;
  double utilityAvoid(double hdg, double spd) const;
  double metricCPA(double) const;

 private:
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain

  double m_osh;
  double m_osv;
  bool   m_osh_set;
  bool   m_osv_set;

  // used to store original stand-on course/speed prior to 17.a.ii actions
  double m_original_course; 

  double m_original_speed;
  bool   m_original_course_set;
  bool   m_original_speed_set;
  
  std::string m_pass_mode;
};


#endif





