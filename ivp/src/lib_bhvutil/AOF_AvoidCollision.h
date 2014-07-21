/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidCollision.h                                 */
/*    DATE: Febuary 9th, 2001 (CPA logic written)                */
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
 
#ifndef AOF_AVOID_COLLISION_HEADER
#define AOF_AVOID_COLLISION_HEADER

#include "AOF_Contact.h"
#include "CPAEngine.h"

class IvPDomain;
class AOF_AvoidCollision: public AOF_Contact {
public:
  AOF_AvoidCollision(IvPDomain);
  ~AOF_AvoidCollision() {}

public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

  double evalROC(double osh, double osv) {
    return(m_cpa_engine.evalROC(osh, osv));
  }
      
 protected:
  double metric(double) const;
  
 protected:
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif



