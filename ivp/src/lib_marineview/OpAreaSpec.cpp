/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: OpAreaSpec.cpp                                       */
/*    DATE: July 6th, 2008                                       */
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

#include <iostream>
#include <cstdlib>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "OpAreaSpec.h"
#include "MBUtils.h"
#include "ColorParse.h"

// As of Release 15.4 this is now set in CMake, defaulting to be defined
// #define USE_UTM 

using namespace std;

//-----------------------------------------------------------
// Constructor

OpAreaSpec::OpAreaSpec()
{
  m_viewable_all    = true;
  m_viewable_labels = true;
  m_line_shade      = 1.0;
  m_label_shade     = 1.0;

  //m_datum_color     = ColorPack("red");
  //m_datum_viewable  = false;
}

//-----------------------------------------------------------
// Procedure: addVertex()

bool OpAreaSpec::addVertex(const std::string& str, 
			   CMOOSGeodesy& geodesy)
{
  vector<string> svector = parseString(str, ',');
  unsigned int vsize = svector.size();

  string xpos, ypos, lat, lon, lwidth, group, label, lcolor;
  string vcolor, dashed, looped;

  for(unsigned int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> ivector = parseString(svector[i], '=');
    if(ivector.size() != 2)
      return(false);
    string left  = tolower(stripBlankEnds(ivector[0]));
    string right = stripBlankEnds(ivector[1]);
    if(left == "xpos")   xpos = right;
    else if(left == "ypos")   ypos = right;
    else if(left == "x")      xpos = right;
    else if(left == "y")      ypos = right;
    else if(left == "lat")    lat = right;
    else if(left == "lon")    lon = right;
    else if(left == "lwidth") lwidth = right;
    else if(left == "group")  group = right;
    else if(left == "label")  label = right;
    else if(left == "lcolor") lcolor = right;
    else if(left == "vcolor") vcolor = right;
    else if(left == "dashed") dashed = tolower(right);
    else if(left == "looped") looped = tolower(right);
  }

  // The position has to be fully specified in terms of either lat/lon
  // of the x-y position in local coords. Otherwise return(false);
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon)))
    if((xpos=="")||(ypos=="")||(!isNumber(xpos))||(!isNumber(ypos)))
      return(false);
  
  if((lwidth!="") && (!isNumber(lwidth)))
    return(false);
  double lwidth_d = atof(lwidth.c_str());
  if(lwidth_d < 0)
    return(false);
  if(lwidth_d < 1)
    lwidth_d = 1.0;

  double xpos_d, ypos_d;
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon))) {
    xpos_d  = atof(xpos.c_str());
    ypos_d  = atof(ypos.c_str());
  }
  else {
    double lat_d = atof(lat.c_str());
    double lon_d = atof(lon.c_str());
#ifdef USE_UTM
    geodesy.LatLong2LocalUTM(lat_d, lon_d, ypos_d, xpos_d);
#else
    geodesy.LatLong2LocalGrid(lat_d, lon_d, ypos_d, xpos_d);
#endif
  }

  bool dashed_b = (dashed == "true");
  bool looped_b = (looped == "true");

  cout << "Adding OpVertex: x=" << xpos_d << ", y=" << ypos_d << endl;

  addVertex(xpos_d, ypos_d, lwidth_d, group, label, lcolor,
	    vcolor, looped_b, dashed_b);

  return(true);
}

//-----------------------------------------------------------
// Procedure: addVertex()

void OpAreaSpec::addVertex(double xpos, double ypos, double lwidth,
			   string group, string label, string lcolor,
			   string vcolor, bool looped, bool dashed)
{
  m_vertex_xpos.push_back(xpos);
  m_vertex_ypos.push_back(ypos);
  m_vertex_lwidth.push_back(lwidth);
  m_vertex_group.push_back(group);
  m_vertex_label.push_back(label);
  m_vertex_dashed.push_back(dashed);
  m_vertex_looped.push_back(looped);

  vector<double> cvect = colorParse(lcolor);
  m_vertex_lcolor.push_back(cvect);
  if(vcolor != "")
    cvect = colorParse(vcolor);
  m_vertex_vcolor.push_back(cvect);
}

//-----------------------------------------------------------
// Procedure: setParam()

bool OpAreaSpec::setParam(const string& param, string value)
{
  if(param == "op_area_viewable_all")
    return(setBooleanOnString(m_viewable_all, value));
  else if(param == "op_area_viewable_labels")
    return(setBooleanOnString(m_viewable_labels, value));
  else if(param == "op_area_line_shade") {
    if(!isNumber(value))
      return(false);
    double dval = atof(value.c_str());
    m_line_shade = vclip(dval, 0, 1);
  }
  else if(param == "op_area_line_shade_mod") {
    if(!isNumber(value))
      return(false);
    double dval = atof(value.c_str());
    m_line_shade *= dval;
    m_line_shade = vclip(m_line_shade, 0, 1);
  }
  else if(param == "op_area_label_shade") {
    if(!isNumber(value))
      return(false);
    double dval = atof(value.c_str());
    m_label_shade = vclip(dval, 0, 1);
  }
  else if(param == "op_area_label_shade_mod") {
    if(!isNumber(value)) 
      return(false);
    double dval = atof(value.c_str());
    m_label_shade *= dval;
    m_label_shade = vclip(m_label_shade, 0, 1);
  }
  else
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: viewable

bool OpAreaSpec::viewable(const string& str) const
{
  if((str == "all") || (tolower(str) == "all"))
    return(m_viewable_all);
  else if((str == "labels") || (tolower(str) == "labels"))
    return(m_viewable_labels);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getXPos

double OpAreaSpec::getXPos(unsigned int ix) const
{
  if(ix < m_vertex_xpos.size())
    return(m_vertex_xpos[ix]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getYPos

double OpAreaSpec::getYPos(unsigned int ix) const
{
  if(ix < m_vertex_ypos.size())
    return(m_vertex_ypos[ix]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getLWidth

double OpAreaSpec::getLWidth(unsigned int ix) const
{
  if(ix < m_vertex_lwidth.size())
    return(m_vertex_lwidth[ix]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getGroup

string OpAreaSpec::getGroup(unsigned int ix) const
{
  if(ix < m_vertex_group.size())
    return(m_vertex_group[ix]);
  return("");
}

//-----------------------------------------------------------
// Procedure: getLabel

string OpAreaSpec::getLabel(unsigned int ix) const
{
  if(ix < m_vertex_label.size())
    return(m_vertex_label[ix]);
  return("");
}

//-----------------------------------------------------------
// Procedure: getDashed

bool OpAreaSpec::getDashed(unsigned int ix) const
{
  if(ix < m_vertex_dashed.size())
    return(m_vertex_dashed[ix]);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getLooped

bool OpAreaSpec::getLooped(unsigned int ix) const
{
  if(ix < m_vertex_looped.size())
    return(m_vertex_looped[ix]);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getLColor

vector<double> OpAreaSpec::getLColor(unsigned int ix) const
{  
  if(ix < m_vertex_lcolor.size())
    return(m_vertex_lcolor[ix]);

  vector<double> grey_vector(3, 0.5);
  return(grey_vector);
}

//-----------------------------------------------------------
// Procedure: getVColor

vector<double> OpAreaSpec::getVColor(unsigned int ix) const
{
  if(ix < m_vertex_vcolor.size())
    return(m_vertex_vcolor[ix]);
  
  vector<double> grey_vector(3, 0.5);
  return(grey_vector);
}










