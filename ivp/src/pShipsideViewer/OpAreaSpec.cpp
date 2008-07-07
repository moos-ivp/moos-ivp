/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OpAreaSpec.cpp                                       */
/*    DATE: July 6th, 2008                                       */
/*****************************************************************/

#include "OpAreaSpec.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "MOOSGeodesy.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

OpAreaSpec::OpAreaSpec()
{
  m_viewable_labels = true;
}

//-----------------------------------------------------------
// Procedure: addVertex()

void OpAreaSpec::addVertex(double xpos, double ypos, double lwidth,
			   string group, string label, string lcolor,
			   string vcolor, bool dashed)
{
  m_vertex_xpos.push_back(xpos);
  m_vertex_ypos.push_back(ypos);
  m_vertex_lwidth.push_back(lwidth);
  m_vertex_group.push_back(group);
  m_vertex_label.push_back(label);
  m_vertex_dashed.push_back(dashed);

  vector<double> cvect = colorParse(lcolor);
  m_vertex_lcolor.push_back(cvect);
  if(vcolor != "")
    cvect = colorParse(vcolor);
  m_vertex_vcolor.push_back(cvect);
}

//-----------------------------------------------------------
// Procedure: addVertex()

bool OpAreaSpec::addVertex(const std::string& str, 
			   CMOOSGeodesy& geodesy)
{
  vector<string> svector = parseString(str, ',');
  unsigned int vsize = svector.size();

  string xpos, ypos, lat, lon, lwidth, group, label, lcolor, vcolor, dashed;
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
  }

  // The position has to be fully specified in terms of either lat/lon
  // of the x-y position in local coords. Otherwise return(false);
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon)))
    if((xpos=="")||(ypos=="")||(!isNumber(xpos))||(!isNumber(ypos)))
      return(false);
  
  if(!isNumber(lwidth))
    return(false);
  double lwidth_d = atof(lwidth.c_str());
  if(lwidth_d <= 0)
    return(false);


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
  
  addVertex(xpos_d, ypos_d, lwidth_d, group, label, lcolor,
	    vcolor, dashed_b);
}

//-----------------------------------------------------------
// Procedure: setParam()

bool OpAreaSpec::setParam(string param, string value)
{
  if(param == "viewable_labels")
    return(setBooleanOnString(m_viewable_labels, value));
  else
    return(false);
  
  return(true);
}

#if 0
//-----------------------------------------------------------
// Procedure: setParam()

bool OpAreaSpec::setParam(string param, double value)
{
  if(param == "mod_scale_all") {
    if((m_marker_scale_global * value) > 0.1)
      m_marker_scale_global *= value;
  }
  else
    return(false);
  return(true);
}

//-----------------------------------------------------------
// Procedure: viewable

bool OpAreaSpec::viewable(const string& str)
{
  if(str == "all")
    return(m_marker_viewable_all);
  else if(str == "labels")
    return(m_marker_viewable_labels);
  return(false);
}
#endif
