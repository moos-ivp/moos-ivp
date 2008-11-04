/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VMarkerSet.cpp                                       */
/*    DATE: July 5th, 2008                                       */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "VMarkerSet.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "MOOSGeodesy.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VMarkerSet::VMarkerSet()
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

bool VMarkerSet::addVMarker(const string& mline,
			    CMOOSGeodesy& geodesy)
{
  vector<string> svector = parseString(mline, ',');
  unsigned int vsize = svector.size();

  cout << "Handling marker line: " << mline << endl;

  string mtype, xpos, ypos, lat, lon, label, colors;
  string scale = "1";
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
    else if(left == "scale")  scale = right;
    else if(left == "lat")    lat = right;
    else if(left == "lon")    lon = right;
    else if(left == "label")  label = right;
    else if(left == "colors") colors = right;
    else if(left == "color")  colors = right;
  }

  if((mtype==""))
    return(false);
  
  // The position has to be fully specified in terms of either lat/lon
  // of the x-y position in local coords. Otherwise return(false);
  if((lat=="")||(lon=="")||(!isNumber(lat))||(!isNumber(lon)))
    if((xpos=="")||(ypos=="")||(!isNumber(xpos))||(!isNumber(ypos)))
      return(false);

  cout << "Accepted Marker" << endl;

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

  double scale_d = atof(scale.c_str());
  if(scale_d < 0)
    scale_d = 0;
  
  return(addVMarker(mtype, xpos_d, ypos_d, scale_d, label, colors));
}


//-----------------------------------------------------------
// Procedure: addVMarker()
//      Note: return(true) if the given vmarker results in a new entry
//            return(false) if the given marker replaces an old entry

bool VMarkerSet::addVMarker(string mtype, double xpos, double ypos,
			    double mscale, string label, string colors)
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

  vector<vector<double> > color_vectors;
  vector<string> svector = parseString(colors, ':');
  vsize = svector.size();
  for(i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<double> cvect = colorParse(svector[i]);
    color_vectors.push_back(cvect);
  }

  if(found_ix == -1) {
    m_marker_type.push_back(mtype);
    m_marker_xpos.push_back(xpos);
    m_marker_ypos.push_back(ypos);
    m_marker_scale.push_back(mscale);
    m_marker_label.push_back(label);
    m_marker_colors.push_back(svector);
    m_marker_color_vectors.push_back(color_vectors);
    return(true);
  }
  else {
    m_marker_xpos[found_ix] = xpos;
    m_marker_ypos[found_ix] = ypos;
    m_marker_scale[found_ix] = mscale;
    m_marker_colors[found_ix] = svector;
    m_marker_color_vectors[found_ix] = color_vectors;
    return(false);
  }
}

//-----------------------------------------------------------
// Procedure: setParam()

bool VMarkerSet::setParam(const string& param, string value)
{
  if(param == "markers_viewable")
    return(setBooleanOnString(m_markers_viewable, value));
  else if(param == "markers_labels_viewable")
    return(setBooleanOnString(m_marker_labels_viewable, value));
  else if(param == "markers_scale_global") {
    if(isNumber(value)) {
      double dval = atof(value.c_str());
      if(dval > 0.1)
	m_marker_scale_global = dval;
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

bool VMarkerSet::viewable(const string& str)
{
  if((str == "all") || (tolower(str) == "all"))
    return(m_markers_viewable);
  else if((str == "labels") || (tolower(str) == "labels"))
    return(m_marker_labels_viewable);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getMarkerType()

string VMarkerSet::getMarkerType(int ix)
{
  if((ix >= 0) && (ix < m_marker_type.size()))
    return(m_marker_type[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerXPos()

double VMarkerSet::getMarkerXPos(int ix)
{
  if((ix >= 0) && (ix < m_marker_xpos.size()))
    return(m_marker_xpos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerYPos()

double VMarkerSet::getMarkerYPos(int ix)
{
  if((ix >= 0) && (ix < m_marker_ypos.size()))
    return(m_marker_ypos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerScale()

double VMarkerSet::getMarkerScale(int ix)
{
  if((ix >= 0) && (ix < m_marker_scale.size()))
    return(m_marker_scale[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerLabel()

string VMarkerSet::getMarkerLabel(int ix)
{
  if((ix >= 0) && (ix < m_marker_label.size()))
    return(m_marker_label[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerColorString

string VMarkerSet::getMarkerColorString(int mix, int cix)
{
  if((mix < 0) && (mix >= m_marker_colors.size()))
    return("");
  
  if((cix < 0) && (cix >= m_marker_colors[mix].size()))
    return("");
  
  return(m_marker_colors[mix][cix]);
}


//-----------------------------------------------------------
// Procedure: getMarkerColorVector

vector<vector<double> > VMarkerSet::getMarkerColorVectors(int mix)
{
  vector<vector<double> > fail_vector;

  if((mix < 0) && (mix >= m_marker_color_vectors.size()))
    return(fail_vector);
  else
    return(m_marker_color_vectors[mix]);
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

vector<string> VMarkerSet::getParamReport() const
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
