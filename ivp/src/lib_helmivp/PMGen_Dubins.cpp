/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Dubins.cpp                                     */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "PMGen_Dubins.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

PMGen_Dubins::PMGen_Dubins()
{
  m_valid = false;
  m_radius     = 30;
  m_spoke_degs = 10;
}

//----------------------------------------------------------------
// Procedure: generate()

PlatModel PMGen_Dubins::generate(double osx, double osy,
				 double osh, double osv)
{
  PlatModel pmodel_holo("holo");

  if(!valid())
    return(pmodel_holo);

  PlatModel pmodel(osx, osy, osh, osv);
  bool ok = init(osx, osy, osh);
  ok = ok && pmodel.setStarSpokes(m_star_spoke_vx, m_star_spoke_vy);
  ok = ok && pmodel.setPortSpokes(m_port_spoke_vx, m_port_spoke_vy);
  if(!ok)
    return(pmodel_holo);
  
  pmodel.setStarSeglrs(m_star_seglrs);
  pmodel.setPortSeglrs(m_port_seglrs);
  pmodel.setSpokeDegs(m_spoke_degs);
  pmodel.setModelType("dubins");
  
  //pmodel.print();
  return(pmodel);
}

//----------------------------------------------------------------
// Procedure: setParam()

bool PMGen_Dubins::setParam(string param, string sval)
{
  // Supported aliases
  if(param == "rad") param = "radius";
  if(param == "degs") param = "spoke_degs";
  
  // Handle the params
  if(param == "radius")
    return(setNonNegDoubleOnString(m_radius, sval));
  else if(param == "spoke_degs")
    return(setPosDoubleOnString(m_spoke_degs, sval));

  else if((param == "mod_radius") && isNumber(sval)) {
    double dval = atof(sval.c_str());
    m_radius += dval;
    if(m_radius < 0)
      m_radius = 0;
    return(true);
  }
  else if((param == "mod_spoke_degs") && isNumber(sval)) {
    double dval = atof(sval.c_str());
    m_spoke_degs += dval;
    if(m_spoke_degs < 0)
      m_spoke_degs = 0;
    m_spoke_degs = angle360(m_spoke_degs);
    return(true);
  }

  else
    return(false);
}

//----------------------------------------------------------------
// Procedure: getParamDbl()

double PMGen_Dubins::getParamDbl(string param) const
{
  if(param == "radius")
    return(m_radius);
  else if("spoke_degs")
    return(m_spoke_degs);
  else if("degs")
    return(m_spoke_degs);

  return(0);
}

//----------------------------------------------------------------
// Procedure: valid()

bool PMGen_Dubins::valid() const
{
  return(true);
}


//----------------------------------------------------------------
// Procedure: init()

bool PMGen_Dubins::init(double osx, double osy, double osh)
{
  if(!valid())
    return(false);
  osh = angle360(osh);
  
  //=============================================================
  // Part 1: Init port and star pts. This is center of circle on
  // port and starboard side, where the center of circle is along
  // ownship beam at a dist of radius meters in either direction.
  double port_ptx = 0;
  double port_pty = 0;
  double star_ptx = 0;
  double star_pty = 0;
  projectPoint(osh+90, m_radius, osx, osy, star_ptx, star_pty);
  projectPoint(osh-90, m_radius, osx, osy, port_ptx, port_pty);

  //=============================================================
  // Part 2: Create the root Seglr. If turn radius=0, done.

  XYSeglr root_seglr(osx, osy, osh);
  m_port_seglrs.clear();
  m_star_seglrs.clear();
  m_port_seglrs.push_back(root_seglr);
  m_star_seglrs.push_back(root_seglr);

  //=============================================================
  // Part 3A: Calculate the Spoke Points (Starboard side)

  m_star_spoke_vx.clear();
  m_star_spoke_vy.clear();
  m_star_spoke_vx.push_back(osx);
  m_star_spoke_vy.push_back(osy);

  for(double degs=m_spoke_degs; degs<180; degs+=m_spoke_degs) {
    double ang = angle360((osh - 90) + degs);
    double vx, vy;
    projectPoint(ang, m_radius, star_ptx, star_pty, vx, vy);
    m_star_spoke_vx.push_back(vx);
    m_star_spoke_vy.push_back(vy);

    XYSeglr seglr = m_star_seglrs.back();
    seglr.addVertex(vx,vy);
    m_star_seglrs.push_back(seglr);
  }

  //=============================================================
  // Part 3B: Calculate the Spoke Points (Port side)
  m_port_spoke_vx.clear();
  m_port_spoke_vy.clear();
  m_port_spoke_vx.push_back(osx);
  m_port_spoke_vy.push_back(osy);

  for(double degs=m_spoke_degs; degs<180; degs+=m_spoke_degs) {
    double ang = angle360((osh + 90) - degs);
    double vx, vy;
    projectPoint(ang, m_radius, port_ptx, port_pty, vx, vy);
    m_port_spoke_vx.push_back(vx);
    m_port_spoke_vy.push_back(vy);

    XYSeglr seglr = m_port_seglrs.back();
    seglr.addVertex(vx,vy);
    m_port_seglrs.push_back(seglr);
  }

  return(true);
}

