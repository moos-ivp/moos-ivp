/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_Markers.cpp                                    */
/*    DATE: July 5th, 2008                                       */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "VPlug_Markers.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "MOOSGeodesy.h"

#define USE_UTM

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_Markers::VPlug_Markers()
{
  m_marker_scale_global    = 1.0;
  m_markers_viewable       = true;
  m_marker_labels_viewable = true;

  m_label_color_default = colorParse("white");
  m_label_color         = m_label_color_default;
  m_label_color_index   = 0;
}

//-----------------------------------------------------------
// Procedure: addVMarker()
//      Note: return(true) if the given vmarker results in a new entry
//            return(false) if the given marker replaces an old entry

bool VPlug_Markers::addVMarker(const string& mline,
			    CMOOSGeodesy& geodesy)
{
  string newline = mline;

  // In case the user provides a line with quotes around it, handle.
  if(isQuoted(mline))
    newline = stripQuotes(mline);

  vector<string> svector = parseString(newline, ',');
  unsigned int vsize = svector.size();

  string mtype, xpos, ypos, lat, lon, label, colors, active;
  string width = "1";
  for(unsigned int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> ivector = parseString(svector[i], '=');
    if(ivector.size() != 2)
      return(false);
    string left  = tolower(stripBlankEnds(ivector[0]));
    string right = stripBlankEnds(ivector[1]);
    if(left == "type")        mtype = right;
    else if(left == "xpos")   xpos = right;
    else if(left == "ypos")   ypos = right;
    else if(left == "x")      xpos = right;
    else if(left == "y")      ypos = right;
    else if(left == "scale")  width = right;
    else if(left == "width")  width = right;
    else if(left == "active") active = right;
    else if(left == "lat")    lat = right;
    else if(left == "lon")    lon = right;
    else if(left == "label")  label = right;
    else if(left == "colors") colors = right;
    else if(left == "color")  colors = right;
  }

  if(mtype=="") {
    cout << "Marker Rejected - improper type" << endl;
    return(false);
  }

  // The position has to be fully specified in terms of either lat/lon
  // of the x-y position in local coords. Otherwise return(false);
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon)))
    if((xpos=="")||(ypos=="")||(!isNumber(xpos))||(!isNumber(ypos))) {
      cout << "Marker Rejected - improper position" << endl;
      return(false);
    }

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

  double width_d = atof(width.c_str());
  if(width_d < 0)
    width_d = 0;

  bool active_b = true;
  if(tolower(active) == "false")
    active_b = false;

  addVMarker(mtype, xpos_d, ypos_d, width_d, label, colors, active_b);
  return(true);
}


//-----------------------------------------------------------
// Procedure: addVMarker()

void VPlug_Markers::addVMarker(string mtype, double xpos, double ypos,
			       double mwid, string label, string colors,
			       bool active)
{
  // First check to see if the given VMarker matches the type and label
  // of a VMarker already added. If the label is the empty string, it
  // will be assumed to be unique.
  int found_ix = -1;
  unsigned int i, vsize = m_marker_type.size();
  if(label != "") {
    for(i=0; i<vsize; i++) {
      if((mtype == m_marker_type[i]) && (label == m_marker_label[i]))
	found_ix = i;
    }
  }

  vector<ColorPack> color_packs;
  vector<string> svector = parseString(colors, ':');
  vsize = svector.size();
  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    ColorPack cpack(svector[i]);
    color_packs.push_back(cpack);
  }

  if(found_ix == -1) {
    m_marker_type.push_back(mtype);
    m_marker_xpos.push_back(xpos);
    m_marker_ypos.push_back(ypos);
    m_marker_width.push_back(mwid);
    m_marker_label.push_back(label);
    m_marker_active.push_back(active);
    m_marker_colors.push_back(svector);
    m_marker_cpacks.push_back(color_packs);
  }
  else {
    m_marker_xpos[found_ix] = xpos;
    m_marker_ypos[found_ix] = ypos;
    m_marker_width[found_ix] = mwid;
    m_marker_active[found_ix] = active;
    m_marker_colors[found_ix] = svector;
    m_marker_cpacks[found_ix] = color_packs;
  }
}

//-----------------------------------------------------------
// Procedure: setParam()

bool VPlug_Markers::setParam(const string& param, string value)
{
  if(param == "markers_viewable")
    return(setBooleanOnString(m_markers_viewable, value));
  else if(param == "markers_labels_viewable")
    return(setBooleanOnString(m_marker_labels_viewable, value));
  else if(param == "markers_scale_global") {
    if(isNumber(value)) {
      double dval = atof(value.c_str());
      m_marker_scale_global = vclip(dval, 0.1, 100);
    }
    else {
      if(value == "smaller")
	m_marker_scale_global *= 0.8;
      else if(value == "bigger")
	m_marker_scale_global *= 1.25;
      else if(value == "reset")
	m_marker_scale_global = 1;
      else
	return(false);
      m_marker_scale_global = vclip(m_marker_scale_global, 0.1, 100);
    }
  }
       
  else if(param == "markers_label_color") {
    if(value == "toggle") {
      m_label_color_index++;
      if(m_label_color_index > 9)
	m_label_color_index = 0;

      if(m_label_color_index == 0) 
	m_label_color = m_label_color_default;
      else if(m_label_color_index == 1)
	m_label_color = colorParse("DarkBlue");
      else if(m_label_color_index == 2)
	m_label_color = colorParse("Yellow");
      else if(m_label_color_index == 3)
	m_label_color = colorParse("Red");
      else if(m_label_color_index == 4)
	m_label_color = colorParse("Green");
      else if(m_label_color_index == 5)
	m_label_color = colorParse("Azure");
      else if(m_label_color_index == 6)
	m_label_color = colorParse("Khaki");
      else if(m_label_color_index == 7)
	m_label_color = colorParse("DarkCyan");
      else if(m_label_color_index == 8)
	m_label_color = colorParse("Blue");
      else
	m_label_color = colorParse("Coral");
    }
    else {
      if(!isColor(value))
	return(false);
      vector<double> cvect = colorParse(value);
      m_label_color_default = cvect;
      m_label_color         = cvect;
      m_label_color_index   = 0;
    }
  }
  else
    return(false);

  return(true);
}

//-----------------------------------------------------------
// Procedure: viewable

bool VPlug_Markers::viewable(const string& str)
{
  if((str == "all") || (tolower(str) == "all"))
    return(m_markers_viewable);
  else if((str == "labels") || (tolower(str) == "labels"))
    return(m_marker_labels_viewable);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getMarkerType()

string VPlug_Markers::getMarkerType(unsigned int ix) const
{
  if(ix < m_marker_type.size())
    return(m_marker_type[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerXPos()

double VPlug_Markers::getMarkerXPos(unsigned int ix) const
{
  if(ix < m_marker_xpos.size())
    return(m_marker_xpos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerYPos()

double VPlug_Markers::getMarkerYPos(unsigned int ix) const
{
  if(ix < m_marker_ypos.size())
    return(m_marker_ypos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerWidth()

double VPlug_Markers::getMarkerWidth(unsigned int ix) const
{
  if(ix < m_marker_width.size())
    return(m_marker_width[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerLabel()

string VPlug_Markers::getMarkerLabel(unsigned int ix) const
{
  if(ix < m_marker_label.size())
    return(m_marker_label[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: active()

bool VPlug_Markers::active(unsigned int ix) const
{
  if(ix < m_marker_active.size())
    return(m_marker_active[ix]);
  else
    return(false);
}

//-----------------------------------------------------------
// Procedure: getMarkerColorString

string VPlug_Markers::getMarkerColorString(unsigned int mix, 
					   unsigned int cix) const
{
  if(mix >= m_marker_colors.size())
    return("");
  
  if(cix >= m_marker_colors[mix].size())
    return("");
  
  return(m_marker_colors[mix][cix]);
}


//-----------------------------------------------------------
// Procedure: getMarkerColorVector

vector<ColorPack> VPlug_Markers::getMarkerColorVectors(unsigned int mix) const
{
  if(mix < m_marker_cpacks.size())
    return(m_marker_cpacks[mix]);
  
  vector<ColorPack> fail_vector;
  return(fail_vector);
}


//-----------------------------------------------------------
// Procedure: getParamReport
//   Purpose: Return a set of strings where each is of the type:
//            "parameter = value".
//            The set returned should be enough to bring future
//            instantiations back to the same current state 
//            should the vector param-value pairs be re-applied 
//            using the setParam interface. This allows a user
//            to "save his/her preferences" in a file or some
//            other configuration block.

vector<string> VPlug_Markers::getParamReport() const
{
  vector<string> svect;

  svect.push_back("// Parameters for Marker Attributes");

  string val = dstringCompact(doubleToString(m_marker_scale_global));
  svect.push_back("markers_scale_global = " + val);

  val = boolToString(m_markers_viewable);
  svect.push_back("markers_viewable = " + val);

  val = boolToString(m_marker_labels_viewable);
  svect.push_back("marker_labels_viewable = " + val);

  return(svect);

}
