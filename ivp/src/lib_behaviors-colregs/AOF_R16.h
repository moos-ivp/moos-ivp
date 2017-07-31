/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_R16.h                                            */
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
 
#ifndef AOF_R16_HEADER
#define AOF_R16_HEADER

#include "AOF_Contact.h"

class AOF_R16: public AOF_Contact {
public:
  AOF_R16(IvPDomain);
  ~AOF_R16() {};

 public: // virtual functions   
  double evalBox(const IvPBox*) const;

  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
  bool   initialize();
  
 protected: // non-virtual functions

  double metricCPA(double) const;
  double metricCRX(double, double, double) const;

 private:
  int    m_crs_ix;          // Index of "course" variable in IvPDomain
  int    m_spd_ix;          // Index of "speed" variable in IvPDomain 
  bool   m_pass_to_stern;
  bool   m_pass_to_bow;
  bool   m_passing_side_set;

  double m_ok_cn_bow_cross_dist;
  
  double m_osh;
  bool   m_osh_set;
  double m_osh_orig;
  bool   m_osh_orig_set;

};
#endif




