/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_R13.h                                            */
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
 
#ifndef AOF_R13_HEADER
#define AOF_R13_HEADER

#include "AOF_Contact.h"

class AOF_R13: public AOF_Contact {
public:
  AOF_R13(IvPDomain);
  ~AOF_R13() {};

  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, const std::string&);
  bool   setParam(const std::string&, double);
  bool   initialize();
  
 protected: // non-virtual functions
  double metricCPA(double) const;
  double metricPassesSide(double, double, double) const;

 private:
  int    m_crs_ix;             // Index of "course" variable in IvPDomain
  int    m_spd_ix;             // Index of "speed" variable in IvPDomain 
  bool   m_pass_to_port;
  bool   m_passing_side_set;
  bool   m_port_of_contact;

  double m_cn_to_os_rel_bng;
};

#endif
