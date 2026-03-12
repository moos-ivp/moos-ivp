/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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

#include "AOF_ContactX.h"

class AOF_R16: public AOF_ContactX {
public:
  AOF_R16(IvPDomain, CPXEngine*);
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
  bool   m_pass_to_stern;
  bool   m_pass_to_bow;
  bool   m_passing_side_set;

  double m_ok_cn_bow_cross_dist;
  
  bool   m_pts_port_turns_ok; // Added Oct1124
};
#endif





