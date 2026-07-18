/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AOF_ContactX.h                                       */
/*    DATE: May 17th, 2013 (Generalizing over existing classes)  */
/*    DATE: Jan 24th, 2026 Simplification with CPXEngine         */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef AOF_CONTACT_X_HEADER
#define AOF_CONTACT_X_HEADER

#include "IvPDomain.h"
#include "AOF.h"
#include "CPXEngine.h"

class AOF_ContactX: public AOF {
public:
  AOF_ContactX(IvPDomain, CPXEngine*);
  ~AOF_ContactX() {}

public: // virtual functions   
  virtual double evalPoint(const std::vector<double>&) const {return(0);}
  virtual bool   setParam(const std::string&, double);
  virtual bool   initialize();

protected:
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain 
  double m_tol;     // Ownship Time on Leg

  double m_collision_distance;
  double m_all_clear_distance;

  bool   m_tol_set;
  bool   m_collision_distance_set;
  bool   m_all_clear_distance_set;

  CPXEngine *m_cpa_engine;
};

#endif

