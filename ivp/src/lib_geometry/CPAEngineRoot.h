/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPAEngineB.h                                         */
/*    DATE: June 12th 2017                                       */
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
 
#ifndef CPA_ENGINE_ROOT_HEADER
#define CPA_ENGINE_ROOT_HEADER

#include <vector>

class CPAEngineRoot {
public:
  CPAEngineRoot(double cny=0, double cnx=0, double cnh=0,
		double cnv=0, double osy=0, double osx=0);

  virtual ~CPAEngineRoot() {}

public:    
  virtual double evalCPAX(double osh, double osv, double ost) {return(0);}
  virtual double evalCPA(double osh, double osv, double ost) const {return(0);}
  virtual void   reset(double cny, double cnx, double cnh,
		       double cnv, double osy, double osx);

  unsigned int getOpenings() const {return(m_openings);}
  unsigned int getOpeningsEarly() const {return(m_openings_early);}
  
 protected:
  double degToRadiansX(double deg) const {return((deg/180.0) * 3.14159265359);}
  
 protected: // Config parameters
  double m_osx;    // Ownship Lon position at time Tm.
  double m_osy;    // Ownship Lat position at time Tm.
  double m_cnx;    // Contact Lon position at time Tm.
  double m_cny;    // Contact Lat position at time Tm.
  double m_cnv;    // Contact Speed in kts.
  double m_cnh;    // Contact Course in degrees (0-359).

  // Keep track of maneuvers evaluated to be opening. Also the number
  // of times the opening was detected early.
  unsigned int m_openings;
  unsigned int m_openings_early;
};

#endif





