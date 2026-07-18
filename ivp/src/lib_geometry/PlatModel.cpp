/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PlatModel.cpp                                        */
/*    DATE: July 26th, 2023                                      */
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

#include <cmath>
#include "PlatModel.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

PlatModel::PlatModel(string model_type)
{
  m_model_type = model_type;
  
  m_osx = 0;  m_osx_set = false;
  m_osy = 0;  m_osy_set = false;
  m_osh = 0;  m_osh_set = false;
  m_osv = 0;  m_osv_set = false;

  m_spoke_degs = 0;
  m_id = 0;
}

//----------------------------------------------------------------
// Constructor()

PlatModel::PlatModel(double osx, double osy, double osh, double osv)
{
  setOSX(osx);
  setOSY(osy);
  setOSH(osh);
  setOSV(osv);

  m_spoke_degs = 0;
  m_id = 0;
}

//----------------------------------------------------------------
// Procedure: setOSH()

void PlatModel::setOSH(double osh)
{
  m_osh = angle360(osh);
  m_osh_set = true;  
}

//----------------------------------------------------------------
// Procedure: setOSV()

void PlatModel::setOSV(double osv)
{
  if(osv < 0)
    osv = 0;
  m_osv = osv;
  m_osv_set = true;
}

//----------------------------------------------------------------
// Procedure: setPose()

void PlatModel::setPose(double osx, double osy, double osh, double osv)
{
  setOSX(osx);
  setOSY(osy);
  setOSH(osh);
  setOSV(osv);
}

//----------------------------------------------------------------
// Procedure: getPoints()
//   Purpose: Return the "spoke" points either port or starboard

vector<XYPoint> PlatModel::getPoints(string param)
{
  vector<XYPoint> rvector;

  if(param == "port_spoke") {
    // Sanity check
    if(m_port_spoke_vx.size() != m_port_spoke_vy.size())
      return(rvector);
    
    for(unsigned int i=0; i<m_port_spoke_vx.size(); i++) {
      XYPoint point(m_port_spoke_vx[i], m_port_spoke_vy[i]);
      point.set_label("mp_" + uintToString(i));
      rvector.push_back(point);
    }
    return(rvector);
  }

  else if(param == "star_spoke") {
    // Sanity check
    if(m_star_spoke_vx.size() != m_star_spoke_vy.size())
      return(rvector);
    
    for(unsigned int i=0; i<m_star_spoke_vx.size(); i++) {
      XYPoint point(m_star_spoke_vx[i], m_star_spoke_vy[i]);
      point.set_label("ms_" + uintToString(i));
      rvector.push_back(point);
    }
    return(rvector);
  }
  
  return(rvector);
}

//----------------------------------------------------------------
// Procedure: getSpokeDists()
#if 0
vector<double> PlatModel::getSpokeDists(string param)
{
  vector<double> rvector;

  if(param == "port") 
    return(m_port_spoke_dist);
  else if(param == "star")
    return(m_star_spoke_dist);
  
  return(rvector);
}
#endif

//----------------------------------------------------------------
// Procedure: setCachePtCPA()

void PlatModel::setCachePtCPA(bool port, unsigned int ix,
			      XYPoint cpa_pt)
{
  if(port && (ix < m_port_seglrs.size()))
    m_port_seglrs[ix].setCacheCPAPoint(cpa_pt);
  else if(ix < m_star_seglrs.size())
    m_star_seglrs[ix].setCacheCPAPoint(cpa_pt);
}
	
//----------------------------------------------------------------
// Procedure: setCacheDistCPA()

void PlatModel::setCacheDistCPA(bool port, unsigned int ix,
				double cpa)
{
  if(port && (ix < m_port_seglrs.size()))
    m_port_seglrs[ix].setCacheCPA(cpa);
  else if(ix < m_star_seglrs.size())
    m_star_seglrs[ix].setCacheCPA(cpa);
}
	
//----------------------------------------------------------------
// Procedure: setCacheStemCPA()

void PlatModel::setCacheStemCPA(bool port, unsigned int ix,
				double stem_cpa)
{
  if(port && (ix < m_port_seglrs.size()))
    m_port_seglrs[ix].setCacheStemCPA(stem_cpa);
  else if(ix < m_star_seglrs.size())
    m_star_seglrs[ix].setCacheStemCPA(stem_cpa);
}
	
//----------------------------------------------------------------
// Procedure: valid()

bool PlatModel::valid() const
{
  if(!m_osx_set || !m_osy_set || !m_osh_set || !m_osv_set) {
    return(false);
  }
    
  return(true);
}


//----------------------------------------------------------------
// Procedure: getTurnSeglr()

XYSeglr PlatModel::getTurnSeglr(double hdg) const
{
  // Default seglr is a single ray originating from ownship current
  // position in the direction of the given heading. Essentially a
  // holonomic instantaneous turn.
  XYSeglr seglr(m_osx, m_osy, hdg);

  // Sanity checks
  if(m_star_spoke_vx.size() != m_star_spoke_vy.size())
    return(seglr);
  if(m_port_spoke_vx.size() != m_port_spoke_vy.size())
    return(seglr);
  if(m_star_spoke_vx.size() == 0)
    return(seglr);
  if(m_port_spoke_vx.size() == 0)
    return(seglr);

  double angle_diff = angleDiff(m_osh, hdg);
  double dsegs = (angle_diff / m_spoke_degs) + 0.5;
  double isegs = (int)(dsegs);

  // Sanity checks that should never occur, but check anyway
  if(isegs < 0)
    isegs = 0;
  unsigned int uisegs = (unsigned int)(isegs);
  
  bool port_turn = portTurn(m_osh, hdg);

  if(port_turn) {
    // Sanity check
    if(uisegs >= m_port_spoke_vx.size())
      uisegs  = m_port_spoke_vx.size()-1;
    seglr = m_port_seglrs[uisegs];
  }
  else {
    // Sanity check
    if(uisegs >= m_star_spoke_vx.size())
      uisegs  = m_star_spoke_vx.size()-1;
    seglr = m_star_seglrs[uisegs];
  }
  
  seglr.setRayAngle(hdg);
  return(seglr);
}

//----------------------------------------------------------------
// Procedure: getSpec()

string PlatModel::getSpec() const
{
  string str = "id=" + uintToString(m_id);
  str += ",type=" + m_model_type;
  str += ",osx=" + doubleToStringX(m_osx,1);
  str += ",osy=" + doubleToStringX(m_osy,1);
  str += ",osh=" + doubleToStringX(m_osh,1);
  str += ",osv=" + doubleToStringX(m_osv,1);

  str += ",spokes:";
  str += uintToString(m_star_spoke_vx.size());
  str += ":";
  str += uintToString(m_port_spoke_vx.size());
  return(str);
}

//----------------------------------------------------------------
// Procedure: setStarSpokes()

bool PlatModel::setStarSpokes(const vector<double>& vx,
			      const vector<double>& vy)
{
  if(vx.size() != vy.size())
    return(false);

  m_star_spoke_vx = vx;
  m_star_spoke_vy = vy;

  // Set the spoke distances. For index i, this is the distance from
  // the ith vertex to the previous vertex. The first distance at i=0
  // is just zero. The ith vertex is presumed to be ownship curr pos.
  
  m_star_logs.clear();
  double dist_so_far = 0;
  for(unsigned int i=0; i<vx.size(); i++) {
    SpokeLog log("s" + intToString(i));

    if(i==0) // first spoke pt is just osx,osy curr position
      log.setDist(0);
    else {
      double vx  = m_star_spoke_vx[i];
      double vy  = m_star_spoke_vy[i];
      double pvx = m_star_spoke_vx[i-1];
      double pvy = m_star_spoke_vy[i-1];
      double dist = hypot(vx-pvx, vy-pvy);
      dist_so_far += dist;
      log.setDist(dist_so_far);
    }

    m_star_logs.push_back(log);
  }

  return(true);
}

//----------------------------------------------------------------
// Procedure: setPortSpokes()

bool PlatModel::setPortSpokes(const vector<double>& vx,
			      const vector<double>& vy)
{
  if(vx.size() != vy.size())
    return(false);

  m_port_spoke_vx = vx;
  m_port_spoke_vy = vy;

  // Set the spoke distances. For index i, this is the distance from
  // the ith vertex to the previous vertex. The first distance at i=0
  // is just zero. The ith vertex is presumed to be ownship curr pos.

  m_port_logs.clear();
  double dist_so_far = 0;
  for(unsigned int i=0; i<vx.size(); i++) {
    SpokeLog log("p" + intToString(i));

    if(i==0) // first spoke pt is just osx,osy curr position
      log.setDist(0);
    else {
      double vx  = m_port_spoke_vx[i];
      double vy  = m_port_spoke_vy[i];
      double pvx = m_port_spoke_vx[i-1];
      double pvy = m_port_spoke_vy[i-1];
      double dist = hypot(vx-pvx, vy-pvy);
      dist_so_far += dist;
      log.setDist(dist_so_far);
    }

    m_port_logs.push_back(log);    
  }
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: setStarSeglrs()

void PlatModel::setStarSeglrs(const vector<XYSeglr>& seglrs)
{
  m_star_seglrs = seglrs;
}

//----------------------------------------------------------------
// Procedure: setPortSeglrs()

void PlatModel::setPortSeglrs(const vector<XYSeglr>& seglrs)
{
  m_port_seglrs = seglrs;
}

//----------------------------------------------------------------
// Procedure: getClosestSpokeLog()

SpokeLog PlatModel::getClosestSpokeLog(double px, double py) const
{
  SpokeLog closest_slog;
  if(m_star_logs.size() != m_star_spoke_vx.size())
    return(closest_slog);
  if(m_port_logs.size() != m_port_spoke_vx.size())
    return(closest_slog);
  
  double closest_dist = -1;

  for(unsigned int i=0; i<m_star_spoke_vx.size(); i++) {
    double vx = m_star_spoke_vx[i];
    double vy = m_star_spoke_vy[i];
    double dist = hypot(px-vx, py-vy);
    if((closest_dist < 0) || (dist < closest_dist)) {
      closest_slog = m_star_logs[i];
      closest_dist = dist;
    }
  }

  for(unsigned int i=0; i<m_port_spoke_vx.size(); i++) {
    double vx = m_port_spoke_vx[i];
    double vy = m_port_spoke_vy[i];
    double dist = hypot(px-vx, py-vy);
    if((closest_dist < 0) || (dist < closest_dist)) {
      closest_slog = m_port_logs[i];
      closest_dist = dist;
    }
  }

  return(closest_slog);
}

//----------------------------------------------------------------
// Procedure: print()

void PlatModel::print() const
{
  cout << "id: " << m_id << endl;
  cout << " type: " << m_model_type << endl;
  cout << " osx: " << doubleToString(m_osx,1) << endl;
  cout << " osy: " << doubleToString(m_osy,1) << endl;
  cout << " osh: " << doubleToString(m_osh,1) << endl;
  cout << " osv: " << doubleToString(m_osv,1) << endl;

  cout << " osx_set: " << boolToString(m_osx_set) << endl;
  cout << " osy_set: " << boolToString(m_osy_set) << endl;
  cout << " osh_set: " << boolToString(m_osh_set) << endl;
  cout << " osv_set: " << boolToString(m_osv_set) << endl;
  cout << " total spokes star: " << m_star_spoke_vx.size() << endl;
  cout << " total spokes port: " << m_port_spoke_vx.size() << endl;
  cout << " total seglrs star: " << m_star_seglrs.size() << endl;
  cout << " total seglrs port: " << m_port_seglrs.size() << endl;
  cout << " degs: " << m_spoke_degs << endl;
}

//----------------------------------------------------------------
// Procedure: stringToPlatModel()

PlatModel stringToPlatModel(string str)
{
  PlatModel null_model;
  PlatModel good_model;
  
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if((param == "osx") && isNumber(value))
      good_model.setOSX(atof(value.c_str()));
    else if((param == "osy") && isNumber(value))
      good_model.setOSY(atof(value.c_str()));
    else if((param == "osh") && isNumber(value))
      good_model.setOSH(atof(value.c_str()));
    else if((param == "osv") && isNumber(value))
      good_model.setOSV(atof(value.c_str()));
    else
      return(null_model);
  }
  return(good_model);
}
