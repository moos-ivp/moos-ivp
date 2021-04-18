/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: WallEngine.h                                         */
/*    DATE: Oct 28th 2018                                        */
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
 
#ifndef WALL_ENGINE_HEADER
#define WALL_ENGINE_HEADER

#include <vector>
#include "XYSegList.h"
#include "DubinsCache.h"
#include "ProxPoint.h"

class WallEngine {
public:
  WallEngine();
  ~WallEngine() {};

  bool setEngine(double osy, double osx, double osh, double radius,
		 const std::vector<XYSegList>& walls,
		 unsigned int hdg_choices=360);
  
  double getXCPA(double osh, double osv,
		 double ttc_base, double ttc_rate) const;

  double getMinRangeToWall() const {return(m_min_range_to_wall);}

  
private:
  void   buildWallSegCache();
  void   buildBaseProxCache();

  void   buildProxCache();
  void   buildHitCacheDubins();

  std::vector<ProxPoint> getArcProxPoints(unsigned int ix);

private:  // Config vars
  double m_osx;
  double m_osy;
  double m_osh;
  double m_turn_radius;
  std::vector<XYSegList> m_walls;

  double m_prox_thresh;
  
  unsigned int m_hdg_choices;

  void print(unsigned int, unsigned int) const;
  
private:  // State vars
  DubinsCache m_dcache;

  std::vector<double> m_port_wsegs_x1;
  std::vector<double> m_port_wsegs_y1;  
  std::vector<double> m_port_wsegs_x2;
  std::vector<double> m_port_wsegs_y2;  

  std::vector<double> m_star_wsegs_x1;
  std::vector<double> m_star_wsegs_y1;
  std::vector<double> m_star_wsegs_x2;
  std::vector<double> m_star_wsegs_y2;
  
  std::vector<ProxPoint> m_port_prox_cache;
  std::vector<ProxPoint> m_star_prox_cache;

  std::vector<std::vector<ProxPoint> > m_prox_cache;

  
  
  double m_min_range_to_wall;
  
  std::vector<std::vector<double> > m_cache_cpa; 
  std::vector<std::vector<double> > m_cache_dcpa; 
  
};

#endif
