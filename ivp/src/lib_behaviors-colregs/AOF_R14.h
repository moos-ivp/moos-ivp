/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_R14.h                                            */
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
 
#ifndef AOF_R14_HEADER
#define AOF_R14_HEADER

#include "AOF_Contact.h"

class AOF_R14: public AOF_Contact {
 public:
  AOF_R14(IvPDomain);
  ~AOF_R14() {};
  
 public: // virtual functions   
  //double evalPoint(const std::vector<double>&) const;
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();
  
 protected: // non-virtual functions
  double calculateInitialMaxBngRate() const;

  double metric(double) const;
  double metric2(double, double) const;
  double metric3(double, double) const;

 protected:
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain 

  double m_initial_bng_rate;

};



#endif





