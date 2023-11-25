/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WormHoleSet.cpp                                      */
/*    DATE: Jan 21st 2021                                        */
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

#include <cstdlib>
#include <iostream>
#include "WormHoleSet.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

WormHoleSet::WormHoleSet()
{
  // Init config vars
  m_tunnel_time = 10;  // seconds

  // Init state vars
  m_worm_hole_state  = "normal";    // or fading, emerging
  m_worm_hole_tstamp = 0;           
  m_transparency     = 1;           // Range [0,1]

  m_inx = 0;   // Observed wormhole entry point
  m_iny = 0;
}

//----------------------------------------------------------------
// Procedure: setTunnelTime()

void WormHoleSet::setTunnelTime(double tunnel_time)
{
  if(tunnel_time < 0)
    tunnel_time = 0;

  m_tunnel_time = tunnel_time;  
}

//----------------------------------------------------------------
// Procedure: setTransparency()

void WormHoleSet::setTransparency(double transp)
{
  if(transp < 0)
    transp = 0;
  else if(transp > 1)
    transp = 1;
  
  m_transparency = transp;
}

//----------------------------------------------------------------
// Procedure: addWormHoleConfig()

bool WormHoleSet::addWormHoleConfig(std::string str)
{
  string tag = tokStringParse(str, "tag", ',', '=');
  if(tag == "")
    return(false);
  tag = tolower(tag);

  unsigned int whix = 0;
  bool new_worm_hole = true;
  for(unsigned int i=0; i<m_worm_hole_tags.size(); i++) {
    if(tag == m_worm_hole_tags[i]) {
      whix = i;
      new_worm_hole = false;
    }
  }
  if(new_worm_hole) {
    WormHole new_worm_hole(tag);
    m_worm_holes.push_back(new_worm_hole);
    m_worm_hole_tags.push_back(tag);
    whix = m_worm_holes.size() - 1;
  }

  WormHole worm_hole = m_worm_holes[whix];

  vector<string> svector = parseStringZ(str, ',', "{");
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    cout << "param:[" << param << "], value:[" << value << "]" << endl;
    
    if(param == "madrid_poly") {
      if(isBraced(value))
	value = stripBraces(value);
      else if(!strBegins(value, "pts="))
	value = "pts={" + value + "}";
      cout << "  value:[" << value << "]" << endl; 

      XYPolygon poly = string2Poly(value);
      poly.set_label(tag + "_madrid");
      poly.set_color("fill", "white");
      poly.set_label_color("invisible");

      poly.set_transparency(0.1);
      poly.set_color("edge", "invisible");
      poly.set_color("vertex", "invisible");

      if(!poly.is_convex())
	return(false);
      worm_hole.setMadridPoly(poly);
    }
    else if(param == "weber_poly") {
      if(isBraced(value))
	value = stripBraces(value);
      else if(!strBegins(value, "pts="))
	value = "pts={" + value + "}";
      cout << "  value:[" << value << "]" << endl; 

      XYPolygon poly = string2Poly(value);

      poly.set_label(tag + "_weber");
      poly.set_color("fill", "white");
      poly.set_label_color("invisible");

      poly.set_transparency(0.1);
      poly.set_color("edge", "invisible");
      poly.set_color("vertex", "invisible");

      if(!poly.is_convex())
	return(false);
      worm_hole.setWeberPoly(poly);
    }
    else if(param == "connection") {
      value = tolower(value);
      if((value != "both") && (value != "from_madrid")
	 && (value != "from_weber"))
	return(false);
      worm_hole.setConnectionType(value);
    }
    else if(param == "delay") {
      if(!isNumber(value))
	return(false);
      double dval = atof(value.c_str());
      if(dval < 0)
	return(false);
      worm_hole.setDelay(dval);
    }
    else if(param == "id_change") {
      value = tolower(value);
      if(!isBoolean(value))
	return(false);
      bool bval = (value == "true");
      worm_hole.setIDChange(bval);
    }
    else if(param != "tag")
      return(false);
  }

  m_worm_holes[whix] = worm_hole;
  
  cout << "returning true!" << endl;
  return(true);
}


//----------------------------------------------------------------
// Procedure: getConfigSummary()

vector<string> WormHoleSet::getConfigSummary() const
{
  vector<string> set_summary;
  string str = "Total WormHoles: " + uintToString(m_worm_holes.size());
  set_summary.push_back(str);
  
  for(unsigned int i=0; i< m_worm_holes.size(); i++) {
    WormHole worm_hole = m_worm_holes[i];
    vector<string> wh_summary = worm_hole.getConfigSummary();
    for(unsigned int j=0; j<wh_summary.size(); j++)
      set_summary.push_back(wh_summary[j]);
  }
  
  return(set_summary);
}


//----------------------------------------------------------------
// Procedure: getWormHole()

WormHole WormHoleSet::getWormHole(unsigned int ix)
{
  WormHole null_worm_hole;
  if(ix >= m_worm_holes.size())
    return(null_worm_hole);
  
  return(m_worm_holes[ix]);
}


//----------------------------------------------------------------
// Procedure: getWormHole()

WormHole WormHoleSet::getWormHole(string tag)
{
  WormHole null_worm_hole;

  // Sanity Check
  if(m_worm_holes.size() != m_worm_hole_tags.size())
    return(null_worm_hole);

  for(unsigned int i=0; i<m_worm_hole_tags.size(); i++) {
    if(tag == m_worm_hole_tags[i])
      return(m_worm_holes[i]);
  }

  return(null_worm_hole);
}


//----------------------------------------------------------------
// Procedure: apply()
//   Returns: true if ownship position was transported

bool WormHoleSet::apply(double curr_time, double osx, double osy,
			double& newx, double& newy)
{ 
  // Case 1: First handle where ownship is emerging from a wormhole
  // In this case the only task is to adjust the transparency
  if(m_worm_hole_state == "emerging") {
    double delta_time = curr_time - m_worm_hole_tstamp;

    if(delta_time > (m_tunnel_time/2)) {
      m_worm_hole_state  = "normal";
      m_worm_hole_tstamp = curr_time;
      m_transparency = 1;
    }
    else {
      double pct = delta_time / (m_tunnel_time/2);
      m_transparency = pct;
    }
    return(false);
  }

  // Case 2: First encounter with a wormhole
  if(m_worm_hole_state == "normal") {
    string worm_hole_tag = findWormHoleEntry(osx, osy);
    if(worm_hole_tag == "")
      return(false);
    else {
      m_worm_hole_state  = "fading";
      m_worm_hole_tstamp = curr_time;
      m_worm_hole_tag    = worm_hole_tag;
      m_inx = osx;
      m_iny = osy;
    }
  }

  // Case 3: Ownship is fading into a wormhole
  if(m_worm_hole_state == "fading") {
    double delta_time = curr_time - m_worm_hole_tstamp;
    if(delta_time >= (m_tunnel_time/2)) {
      m_worm_hole_state  = "emerging";
      m_worm_hole_tstamp = curr_time;
      m_transparency = 0;

      WormHole worm_hole = getWormHole(m_worm_hole_tag);
      string connection_type = worm_hole.getConnectionType();
      if(connection_type == "from_weber") {
	worm_hole.crossPositionWeberToMadrid(m_inx,m_iny, newx,newy);
	return(true);
      }
      if(connection_type == "from_madrid") {
	worm_hole.crossPositionMadridToWeber(m_inx,m_iny, newx,newy);
	return(true);
      }	
    }
    else {
      double pct = delta_time / (m_tunnel_time/2);
      m_transparency = 1- pct;
      return(false);
    }
  }

  return(false);
}


//----------------------------------------------------------------
// Procedure: findWormHoleEntry()

string WormHoleSet::findWormHoleEntry(double osx, double osy)
{
  // Sanity Check
  if(m_worm_holes.size() != m_worm_hole_tags.size())
    return("");

  // Check each worm_hole, and if ownship is in entry polygon
  // then return the tag of that worm_hole
  for(unsigned int i=0; i<m_worm_holes.size(); i++) {

    WormHole worm_hole = m_worm_holes[i];
    
    string connection_type = worm_hole.getConnectionType();
    if(connection_type == "from_weber") {
      XYPolygon weber_poly = worm_hole.getWeberPoly();
      if(weber_poly.contains(osx, osy))
	return(m_worm_hole_tags[i]);
    }
    if(connection_type == "from_madrid") {
      XYPolygon madrid_poly = worm_hole.getMadridPoly();
      if(madrid_poly.contains(osx, osy)) 
	return(m_worm_hole_tags[i]);
    }
  }    
  
  return("");
}






