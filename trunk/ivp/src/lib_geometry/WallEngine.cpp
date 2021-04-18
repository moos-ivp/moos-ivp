/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: WallEngine.cpp                                       */
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

#include <iostream>
#include <cmath> 
#include <algorithm>
#include "WallEngine.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "ArcUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

WallEngine::WallEngine() 
{
  // Initialize config vars
  m_osx = 0;
  m_osy = 0;
  m_osh = 0;
  m_turn_radius = 0;
  m_hdg_choices = 360;

  m_prox_thresh = 10;
  
  // Initialize state vars
  m_min_range_to_wall = -1;
}

//----------------------------------------------------------------
// Procedure: setEngine

bool WallEngine::setEngine(double osx, double osy, double osh, double radius,
			   const vector<XYSegList>& walls,
			   unsigned int hdg_choices)
{
  if(walls.size() == 0)
    return(false);

  m_osx = osx;
  m_osy = osy;
  m_osh = osh;
  m_turn_radius = radius;

  for(unsigned int i=0; i<walls.size(); i++) {
    XYSegList segl = walls[i];
    if(segl.size() == 0)
      return(false);
    if(segl.segs_cross(false))
      return(false);
  }

  m_hdg_choices = hdg_choices;
  m_walls = walls;

  m_dcache.setParams(osx, osy, osh, radius);
  m_dcache.buildCache(m_hdg_choices);

  //buildWallSegCache();
  buildBaseProxCache();
  buildProxCache();
  
  buildHitCacheDubins();

  for(unsigned int i=0; i<walls.size(); i++) {
    double dist = distPointToSegl(osx, osy, walls[i]);
    if((m_min_range_to_wall < 0) || (dist < m_min_range_to_wall))
      m_min_range_to_wall = dist;
  }

  print(185, 230);

  return(true);
}

//----------------------------------------------------------------
// Procedure: getXCPA

double WallEngine::getXCPA(double osh, double osv,
			   double ttc_base, double ttc_rate) const
{
  if((osh >= 360) || (osh < 0))
    osh = angle360(osh);

  // Fix-me this needs to change if m_hdg_choices != 360
  unsigned int uint_osh = (unsigned int)(osh);

  // If nothing in the hit cache for this heading indicate so
  // by returnig a negative val. Otherwise ret vals always positive
  if(m_cache_cpa[uint_osh].size() == 0) 
    return(-1);
  
  // Hack for now
  if(osv <= 0)
    osv = 0.1;

#if 1
  if(uint_osh >= m_prox_cache.size())
    return(-1);
  
  vector<ProxPoint> vppts = m_prox_cache[uint_osh];
  double min_xcpa = -1;
  for(unsigned int i=0; i<vppts.size(); i++) {
    double this_xcpa = vppts[i].getCPA();

    double time_to_cpa = vppts[i].getCPADist() / osv;
    if(time_to_cpa > ttc_base) {
      double delta = time_to_cpa - ttc_base;
      double xfactor = ttc_rate * delta;
      this_xcpa += xfactor;
    }
    if((min_xcpa < 0) || (this_xcpa < min_xcpa))
      min_xcpa = this_xcpa;
  }
#endif

#if 0
  vector<double> vcpa  = m_cache_cpa[uint_osh];
  vector<double> vdcpa = m_cache_dcpa[uint_osh];

  double min_xcpa = -1;
  for(unsigned int i=0; i<vcpa.size(); i++) {
    double this_xcpa = vcpa[i];

    double time_to_cpa = vdcpa[i] / osv;
    if(time_to_cpa > ttc_base) {
      double delta = time_to_cpa - ttc_base;
      double xfactor = ttc_rate * delta;
      this_xcpa += xfactor;
    }
    if((min_xcpa < 0) || (this_xcpa < min_xcpa))
      min_xcpa = this_xcpa;
  }
#endif
      
  return(min_xcpa);
}

//----------------------------------------------------------------
// Procedure: buildHitCacheDubins()

void WallEngine::buildHitCacheDubins()
{
  if(!m_dcache.valid())
    return;

  unsigned int dsize = m_dcache.size();
  for(unsigned int i=0; i<dsize; i++) {
    vector<double> vcpa;
    vector<double> vdcpa;
    m_cache_cpa.push_back(vcpa);
    m_cache_dcpa.push_back(vdcpa);
  }

  // Calculate the vcpa and dvcpa's for the arcs

  // Note vcpa and dvcpa's could be built once for full port and
  // starboard turns, then ordered based on distance to the
  // origin. Subsequent queries for these vectors could then specify
  // the distance to the origin and return an ordered subset -
  // potentially much more efficient.

  double thresh = 10;
  for(unsigned int i=0; i<dsize; i++) {

    double ax,ay,ar,lang,rang;
    m_dcache.getArcIX(i, ax,ay,ar, lang,rang);

    bool langle_is_origin = true;
    if(m_dcache.portTurnIX(i))
      langle_is_origin = false;
    
    for(unsigned int j=0; j<m_walls.size(); j++) {
      
      vector<double> vcpa;
      vector<double> vdcpa;
      unsigned int pts = cpasArcSegl(ax,ay,ar, lang,rang,
				     m_walls[j], thresh,
				     langle_is_origin,
				     vcpa, vdcpa);
      
      for(unsigned int j=0; j<pts; j++) {
	m_cache_cpa[i].push_back(vcpa[j]);
	m_cache_dcpa[i].push_back(vdcpa[j]);
      }
    }
  }

  // Calculate the vcpa and dvcpa's for the rays 
  double hdg_delta = 360.0 / (double)(m_hdg_choices);
  for(unsigned int i=0; i<dsize; i++) {
    double ray_angle = hdg_delta * (double)(i);
    double rx, ry;
    m_dcache.getRayIX(i, rx, ry);

    double arc_len = m_dcache.getArcLenIX(i);
    
    for(unsigned int j=0; j<m_walls.size(); j++) {
      vector<double> vcpa;
      vector<double> vdcpa;
      unsigned int pts = cpasRaySegl(rx, ry, ray_angle, m_walls[j],
				     thresh, vcpa, vdcpa);

      //double dist_to_segl = distPointToSegl(rx, ry, m_walls[j]);
      
      for(unsigned int j=0; j<pts; j++) {
	m_cache_cpa[i].push_back(vcpa[j]);
	m_cache_dcpa[i].push_back(vdcpa[j] + arc_len);
	//m_cache_dcpa[i].push_back(dist_to_segl);
      }
    }
  }
}


//----------------------------------------------------------------
// Procedure: getArcProxPoints()

vector<ProxPoint> WallEngine::getArcProxPoints(unsigned int ix)
{
  vector<ProxPoint> ppts;
  if(!m_dcache.valid())
    return(ppts);

  if(ix >= m_dcache.size())
    return(ppts);
  
  double arc_len = m_dcache.getArcLenIX(ix);
  bool port_turn = m_dcache.portTurnIX(ix);

  if(port_turn) {
    bool done = false;
    for(unsigned int i=0; (i<m_port_prox_cache.size()) && !done; i++) {
      //if(arc_len <= m_port_prox_cache[i].getCPADist())
      if(m_port_prox_cache[i].getCPADist() <= arc_len)
	ppts.push_back(m_port_prox_cache[i]);
      else
	done = true;
    }
  }
  else {
    bool done = false;
    for(unsigned int i=0; (i<m_star_prox_cache.size()) && !done; i++) {
      //if(arc_len <= m_star_prox_cache[i].getCPADist())
      if(m_star_prox_cache[i].getCPADist() <= arc_len)
	ppts.push_back(m_star_prox_cache[i]);
      else
	done = true;
    }
  }
  return(ppts);
}

  //----------------------------------------------------------------
// Procedure: buildProxCache()

void WallEngine::buildProxCache()
{
  if(!m_dcache.valid())
    return;

  for(unsigned int i=0; i<m_dcache.size(); i++) {
    vector<ProxPoint> pcache;
    m_prox_cache.push_back(pcache);
  }

  // Part 1: Get the Arc prox points from the base prox caches
  for(unsigned int i=0; i<m_dcache.size(); i++) 
    m_prox_cache[i] = getArcProxPoints(i);

  // Part 2: Get the Ray prox points
  for(unsigned int i=0; i<m_dcache.size(); i++) {
    double rx, ry;
    m_dcache.getRayIX(i, rx, ry);
    double ray_angle = m_dcache.getTurnHdgIX(i);

    double arc_len = m_dcache.getArcLenIX(i);
    
    for(unsigned int j=0; j<m_walls.size(); j++) {
      vector<double> vcpa;
      vector<double> vdcpa;
      unsigned int pts = cpasRaySegl(rx, ry, ray_angle, m_walls[j],
				     m_prox_thresh, vcpa, vdcpa);

      for(unsigned int k=0; k<pts; k++) {
	ProxPoint ppt;
	ppt.setCPA(vcpa[k]);
	ppt.setCPADist(arc_len + vdcpa[k]);
	ppt.setType(2);
	m_prox_cache[i].push_back(ppt);
      }
    }
  }

  // Part 3: Make sure the prox cache for each turn is sorted
  for(unsigned int i=0; i<m_dcache.size(); i++) {
    if(m_prox_cache[i].size() > 0)
      sort(m_prox_cache[i].begin(), m_prox_cache[i].end());
  }
  
}



//----------------------------------------------------------------
// Procedure: buildWallSegCache()
//   Purpose: Convert all the wall seglists into individual line
//            segments. The port cache excludes segments that are
//            completely on the starboard side. And vice versa for
//            the starboard cache. This will make future calculations
//            quicker, and also ensures that, a starboard turn does
//            not consider walls on the port side.

void WallEngine::buildWallSegCache()
{
  for(unsigned int i=0; i<m_walls.size(); i++) {
    // special case: seglist is only one vertex
    if(m_walls[i].size() == 1) {
      double px = m_walls[i].get_vx(0);
      double py = m_walls[i].get_vy(0);

      double ang = relAng(m_osx,m_osy, px,py);
      if((ang >= 0) && (ang <= 180)) {
	m_star_wsegs_x1.push_back(px);
	m_star_wsegs_y1.push_back(py);
	m_star_wsegs_x2.push_back(px);
	m_star_wsegs_y2.push_back(py);
      }
      if((ang >= 180) && (ang < 360)) {
	m_port_wsegs_x1.push_back(px);
	m_port_wsegs_y1.push_back(py);
	m_port_wsegs_x2.push_back(px);
	m_port_wsegs_y2.push_back(py);
      }
    }
    
    // general case: seglist has more than one vertex
    if(m_walls[i].size() > 1) {
      for(unsigned int j=0; j<m_walls[i].size()-1; j++) {
	double x1 = m_walls[i].get_vx(j);
	double y1 = m_walls[i].get_vy(j);
	double x2 = m_walls[i].get_vx(j+1);
	double y2 = m_walls[i].get_vy(j+1);

	double ang1 = relAng(m_osx,m_osy, x1,y1);
	double ang2 = relAng(m_osx,m_osy, x2,y2);
	if(((ang1 >= 0) && (ang1 <= 180)) ||
	   ((ang2 >= 0) && (ang2 <= 180))) { 
	  m_star_wsegs_x1.push_back(x1);
	  m_star_wsegs_y1.push_back(y1);
	  m_star_wsegs_x2.push_back(x2);
	  m_star_wsegs_y2.push_back(y2);
	}
	if(((ang1 >= 180) && (ang1 < 360)) ||
	   ((ang2 >= 180) && (ang2 < 360))) {
	  m_port_wsegs_x1.push_back(x1);
	  m_port_wsegs_y1.push_back(y1);
	  m_port_wsegs_x2.push_back(x2);
	  m_port_wsegs_y2.push_back(y2);
	}
      }
    }
  }
}


//----------------------------------------------------------------
// Procedure: buildBaseProxCache()
//   Purpose: Build a proxpt cache for max port and starboard turns.
//            The cache is sorted based on distance to the origin (os).
//            This cache can then be used to find the proxpts for
//            all lesser turns by just grabbing the proxpts based
//            on distance to ownship.

void WallEngine::buildBaseProxCache()
{
  cout << "Make Base Prox Cache (START)" << endl;
  cout << "  m_prox_thresh:" << m_prox_thresh << endl;
  if(!m_dcache.valid())
    return;

  // The arc representing the full turn(s)
  double ax,ay,ar,lang,rang;

  // Part 1: Make the Starboard ProxPoint Cache
  m_dcache.getMaxStarTurn(ax,ay,ar, lang,rang);

#if 0
  cout << "max_star_turn: " << endl;
  cout << " ax: " << ax << endl;
  cout << " ay: " << ay << endl;
  cout << " ar: " << ar << endl;
  cout << " lang: " << lang << endl;
  cout << " rang: " << rang << endl;
#endif
  
  bool langle_is_origin = true;
  for(unsigned int i=0; i<m_walls.size(); i++) {
    vector<double> vcpa, vdcpa;
    unsigned int pts = cpasArcSegl(ax,ay,ar, lang,rang,
				   m_walls[i], m_prox_thresh,
				   langle_is_origin, vcpa, vdcpa);

    for(unsigned int j=0; j<pts; j++) {
      ProxPoint pxpt;
      pxpt.setCPA(vcpa[j]);
      pxpt.setCPADist(vdcpa[j]);
      pxpt.setType(1);
      if(pxpt.getCPADist() > 0)
	m_star_prox_cache.push_back(pxpt);
    }
  }

  // Part 2: Make the Port ProxPoint Cache
  m_dcache.getMaxPortTurn(ax,ay,ar, lang,rang);

#if 0
  cout << "max_port_turn: " << endl;
  cout << " ax: " << ax << endl;
  cout << " ay: " << ay << endl;
  cout << " ar: " << ar << endl;
  cout << " lang: " << lang << endl;
  cout << " rang: " << rang << endl;
#endif

  langle_is_origin = false;
  for(unsigned int i=0; i<m_walls.size(); i++) {
    vector<double> vcpa, vdcpa;
    unsigned int pts = cpasArcSegl(ax,ay,ar, lang,rang,
				   m_walls[i], m_prox_thresh,
				   langle_is_origin, vcpa, vdcpa);

    for(unsigned int j=0; j<pts; j++) {
      ProxPoint pxpt;
      pxpt.setCPA(vcpa[j]);
      pxpt.setCPADist(vdcpa[j]);
      pxpt.setType(1);
      if(pxpt.getCPADist() > 0)
	m_port_prox_cache.push_back(pxpt);
    }
  }
  
  // Part 3: Sort ProxPoint Caches based on dist from origin (ownship)
  sort(m_star_prox_cache.begin(), m_star_prox_cache.end());
  sort(m_port_prox_cache.begin(), m_port_prox_cache.end());
  cout << "Make Base Prox Cache (END)" << endl;
}



//----------------------------------------------------------------
// Procedure: print()

void WallEngine::print(unsigned int low_ix, unsigned int hgh_ix) const
{
  if(m_cache_cpa.size() == 0) {
    cout << "Empty caches" << endl;
    return;
  }

  cout << "WallEngine::print() " << endl;
  cout << "==========================================" << endl;
  cout << "hdg_choices: " << m_hdg_choices << endl;
  cout << "turn_radius: " << m_turn_radius << endl;
  cout << "low_ix:      " << low_ix << endl;
  cout << "hgh_ix:      " << hgh_ix << endl;
  
  if(low_ix >= m_cache_cpa.size())
    low_ix = m_cache_cpa.size()-1;
  if(hgh_ix >= m_cache_cpa.size())
    hgh_ix = m_cache_cpa.size()-1;
  if(hgh_ix < low_ix)
    hgh_ix = low_ix;

#if 0
  for(unsigned int i=low_ix; i<hgh_ix; i++) {
    cout << "Heading: " << i << endl;
    cout << "arclen:  " << i << " = " << m_dcache.getArcLenIX(i) << endl;
    for(unsigned int j=0; j<m_cache_cpa[i].size(); j++) {
      cout << "cpa[" << j << "]: " << m_cache_cpa[i][j];
      cout << "   dcpa[" << j << "]: " << m_cache_dcpa[i][j] << endl;
    }
  }
#endif

  for(unsigned int i=low_ix; i<=hgh_ix; i++) {
    cout << "Heading: " << i << endl;
    cout << "arclen:  " << i << " = " << m_dcache.getArcLenIX(i) << endl;
    for(unsigned int j=0; j<m_prox_cache[i].size(); j++) {
      cout << "cpa[" << j << "]: " << m_prox_cache[i][j].getCPA();
      cout << "   dcpa[" << j << "]: " << m_prox_cache[i][j].getCPADist();
      cout << "   type[" << j << "]: " << m_prox_cache[i][j].getType() << endl;
    }
  }
}

 
