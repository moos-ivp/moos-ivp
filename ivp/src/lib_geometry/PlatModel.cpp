/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PlatModel.cpp                                        */
/*    DATE: July 26th, 2023                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "PlatModel.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

PlatModel::PlatModel()
{
  m_osx = 0;  m_osx_set = false;
  m_osy = 0;  m_osy_set = false;
  m_osh = 0;  m_osh_set = false;

  m_spoke_degs = 0;
}

//----------------------------------------------------------------
// Constructor()

PlatModel::PlatModel(double osx, double osy, double osh)
{
  m_osx = osx;  m_osx_set = true;
  m_osy = osy;  m_osy_set = true;
  m_osh = osh;  m_osh_set = true;  

  m_spoke_degs = 0;
}

//----------------------------------------------------------------
// Procedure: getPoints()

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
// Procedure: setPose()

void PlatModel::setPose(double osx, double osy, double osh)
{
  m_osx = osx;
  m_osy = osy;
  m_osh = angle360(osh);

  m_osx_set = true;
  m_osy_set = true;
  m_osh_set = true;  
}

//----------------------------------------------------------------
// Procedure: setCache()

void PlatModel::setCache(bool port, unsigned int ix, XYPoint cpa_pt)
{
  if(port && (ix < m_port_seglrs.size()))
    m_port_seglrs[ix].setCacheCPAPoint(cpa_pt);
  else if(ix < m_star_seglrs.size())
    m_star_seglrs[ix].setCacheCPAPoint(cpa_pt);
}
	
//----------------------------------------------------------------
// Procedure: setCache()

void PlatModel::setCache(bool port, unsigned int ix, double cpa)
{
  if(port && (ix < m_port_seglrs.size()))
    m_port_seglrs[ix].setCacheCPA(cpa);
  else if(ix < m_star_seglrs.size())
    m_star_seglrs[ix].setCacheCPA(cpa);
}
	
//----------------------------------------------------------------
// Procedure: valid()

bool PlatModel::valid() const
{
  if(!m_osx_set || !m_osy_set || !m_osh_set)
    return(false);
    
  return(true);
}


//----------------------------------------------------------------
// Procedure: getTurnSeglr()

XYSeglr PlatModel::getTurnSeglr(double hdg) const
{
  XYSeglr seglr;

  
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
  string str = "osx=" + doubleToStringX(m_osx,2);
  str += ",osy=" + doubleToStringX(m_osy,2);
  str += ",osh=" + doubleToStringX(m_osh,2);
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
// Procedure: print()

void PlatModel::print() const
{
  cout << "total spokes star: " << m_star_spoke_vx.size() << endl;
  cout << "total spokes port: " << m_port_spoke_vx.size() << endl;
  cout << "total seglrs star: " << m_star_seglrs.size() << endl;
  cout << "total seglrs port: " << m_port_seglrs.size() << endl;
  cout << "total seglrs port: " << m_spoke_degs << endl;
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
    else
      return(null_model);
  }
  return(good_model);
}


