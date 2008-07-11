/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OpAreaSpec.cpp                                       */
/*    DATE: July 6th, 2008                                       */
/*****************************************************************/

#include <iostream>
#include "OpAreaSpec.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "MOOSGeodesy.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

OpAreaSpec::OpAreaSpec()
{
  m_viewable_all    = true;
  m_viewable_labels = true;
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

  cout << "addVertexA() x:" << xpos << " y:" << ypos
       << "  lwidth:" << lwidth 
       << "  group:" << group << "  label:" << label << "  lcolor:" 
       << lcolor << "  vcolor:" << vcolor << "  dashed:" << dashed
       << "  looped:" << looped << endl;


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
    geodesy.LatLong2LocalGrid(lat_d, lon_d, ypos_d, xpos_d);
  }

  bool dashed_b = (dashed == "true");
  bool looped_b = (looped == "true");

  cout << "addVertexB() x:" << xpos_d << " y:" << ypos_d 
       << "  lwidth:" << lwidth_d 
       << "  group:" << group << "  label:" << label << "  lcolor:" 
       << lcolor << "  vcolor:" << vcolor << "  dashed_b:" << dashed_b
       << "  looped_b:" << looped_b << endl;

  
  addVertex(xpos_d, ypos_d, lwidth_d, group, label, lcolor,
	    vcolor, looped_b, dashed_b);
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

bool OpAreaSpec::setParam(string param, string value)
{
  if(param == "viewable_all")
    return(setBooleanOnString(m_viewable_all, value));
  else if(param == "viewable_labels")
    return(setBooleanOnString(m_viewable_labels, value));
  else
    return(false);
  
  return(true);
}

//-----------------------------------------------------------
// Procedure: viewable

bool OpAreaSpec::viewable(const string& str)
{
  if((str == "all") || (tolower(str) == "all"))
    return(m_viewable_all);
  else if((str == "labels") || (tolower(str) == "labels"))
    return(m_viewable_labels);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getXPos

double OpAreaSpec::getXPos(int ix)
{
  if((ix >= 0) && (ix < m_vertex_xpos.size()))
    return(m_vertex_xpos[ix]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getYPos

double OpAreaSpec::getYPos(int ix)
{
  if((ix >= 0) && (ix < m_vertex_ypos.size()))
    return(m_vertex_ypos[ix]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getLWidth

double OpAreaSpec::getLWidth(int ix)
{
  if((ix >= 0) && (ix < m_vertex_lwidth.size()))
    return(m_vertex_lwidth[ix]);
  return(0);
}

//-----------------------------------------------------------
// Procedure: getGroup

string OpAreaSpec::getGroup(int ix)
{
  if((ix >= 0) && (ix < m_vertex_group.size()))
    return(m_vertex_group[ix]);
  return("");
}

//-----------------------------------------------------------
// Procedure: getLabel

string OpAreaSpec::getLabel(int ix)
{
  if((ix >= 0) && (ix < m_vertex_label.size()))
    return(m_vertex_label[ix]);
  return("");
}

//-----------------------------------------------------------
// Procedure: getDashed

bool OpAreaSpec::getDashed(int ix)
{
  if((ix >= 0) && (ix < m_vertex_dashed.size()))
    return(m_vertex_dashed[ix]);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getLooped

bool OpAreaSpec::getLooped(int ix)
{
  if((ix >= 0) && (ix < m_vertex_looped.size()))
    return(m_vertex_looped[ix]);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getLColor

vector<double> OpAreaSpec::getLColor(int ix)
{
  vector<double> grey_vector;
  grey_vector.push_back(0.5);
  grey_vector.push_back(0.5);
  grey_vector.push_back(0.5);
  
  if((ix >= 0) && (ix < m_vertex_lcolor.size()))
    return(m_vertex_lcolor[ix]);
  return(grey_vector);
}

//-----------------------------------------------------------
// Procedure: getVColor

vector<double> OpAreaSpec::getVColor(int ix)
{
  vector<double> grey_vector;
  grey_vector.push_back(0.5);
  grey_vector.push_back(0.5);
  grey_vector.push_back(0.5);
  
  if((ix >= 0) && (ix < m_vertex_vcolor.size()))
    return(m_vertex_vcolor[ix]);
  return(grey_vector);
}
