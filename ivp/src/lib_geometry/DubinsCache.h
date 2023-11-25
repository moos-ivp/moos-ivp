/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DubinsCache.h                                        */
/*    DATE: Nov 19th 2018                                        */
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
 
#ifndef DUBINS_CACHE_HEADER
#define DUBINS_CACHE_HEADER

#include <vector>
#include "XYSegList.h"
#include "DubinsTurn.h"

class DubinsCache {
public:
  DubinsCache();
  ~DubinsCache() {};

  
public: // Build functions
  bool setParams(double osx, double osy, double osh, double radius);
  bool buildCache(unsigned int hdg_choices=360);

public: // Getters
  bool   getArc(double new_hdg, double& ax, double& ay, double& ar,
	      double& langle, double& rangle) const;
  bool   getRay(double new_hdg, double& px, double& py) const;
  double getArcLen(double new_hdg) const;

  bool   getArcIX(unsigned int ix, double& ax, double& ay, double& ar,
	          double& langle, double& rangle) const;
  bool   getRayIX(unsigned int ix, double& px, double& py) const;
  double getArcLenIX(unsigned int ix) const;

  bool   starTurnIX(unsigned int ix) const;
  bool   portTurnIX(unsigned int ix) const;
  double getTurnHdgIX(unsigned int ix) const;
  
  double getMaxPortTurn(double& ax, double& ay, double& ar,
			double& langle, double& rangle) const;

  double getMaxStarTurn(double& ax, double& ay, double& ar,
			double& langle, double& rangle) const;


  
public: // Analyzers
  bool valid() const {return(m_dturns.size() > 0);}

  unsigned int size() const {return(m_dturns.size());}

private:
  double degToRadiansX(double deg) const
  {return((deg/180.0) * 3.14159265359);}
  
private:  // Config vars
  double m_osx;
  double m_osy;
  double m_osh;
  double m_turn_radius;

  unsigned int m_hdg_choices;

private:  // State vars

  // Indexed on ownship heading index.  
  std::vector<DubinsTurn> m_dturns;
};

#endif

