/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug-GeoShapes.cpp                                  */
/*    DATE: July 9th, 2008                                       */
/*****************************************************************/

#include "VPlug_GeoShapes.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

VPlug_GeoShapes::VPlug_GeoShapes()
{
  m_poly_viewable_all    = true;
  m_poly_viewable_labels = true;
  m_segl_viewable_all    = true;
  m_segl_viewable_labels = true;
  m_grid_viewable_all    = true;
  m_grid_viewable_labels = true;
  m_circ_viewable_all    = true;
  m_circ_viewable_labels = true;

  m_poly_edge_color  = colorParse("khaki");
  m_poly_vert_color  = colorParse("dark_blue");
  m_poly_label_color = colorParse("khaki");
  m_segl_edge_color  = colorParse("orange");
  m_segl_vert_color  = colorParse("dark_green");
  m_segl_label_color = colorParse("orange");
  m_circ_edge_color  = colorParse("yellow");
  m_grid_edge_color  = colorParse("white");

  m_poly_edge_width  = 2.0;
  m_poly_vert_size   = 5.0;
  m_segl_edge_width  = 2.0;
  m_segl_vert_size   = 5.0;
  m_grid_edge_width  = 2.0;
  m_grid_edge_width  = 2.0;
}

//-----------------------------------------------------------
// Procedure: addPolyon, addSegList, addGrid, addCircle

void VPlug_GeoShapes::addPolygon(const XYPolygon& poly)
{
  m_polygons.push_back(poly);
}

void VPlug_GeoShapes::addSegList(const XYSegList& segl)
{
  m_seglists.push_back(segl);
}

void VPlug_GeoShapes::addGrid(const XYGrid& grid)
{
  m_grids.push_back(grid);
}

void VPlug_GeoShapes::addCircle(const XYCircle& circ)
{
  m_circles.push_back(circ);
}

bool VPlug_GeoShapes::addPolygon(const string& poly_str)
{
  XYPoloygon new_poly = stringToPolygon(poly_str);
  if(new_poly.size() == 0)
    return(false);
  m_polygons.push_back(new_poly);
  return(true);
}

bool VPlug_GeoShapes::addSegList(const string& segl_str)
{
  XYSegList new_segl = stringToSegList(segl_str);
  if(new_segl.size() == 0)
    return(false);
  m_seglists.push_back(new_segl);
  return(true);
}


//-----------------------------------------------------------
// Procedure: setParam()
//     Notes: The possibly accepted parameters are unlimited, but 
//            must fit one of the below patterns. The following is 
//            a list (perhaps incomplete) of acceptable parameters.
//    Each accepting a "color string": 
//         poly_edge_color
//         poly_vert_color
//         poly_label_color
//         segl_edge_color
//         segl_vert_color
//         segl_label_color
//         grid_vert_color
//         circ_edge_color
//    Each accepting a "bool string": 
//         poly_viewable_all
//         poly_viewable_labels
//         segl_viewable_all
//         segl_viewable_labels
//         grid_viewable_all
//         grid_viewable_labels
//         circ_viewable_all
//         circ_viewable_labels
//    Each accepting a "double string": 
//         poly_line_width
//         poly_vert_size
//         segl_line_width
//         segl_vert_size
//         grid_line_width
//         circ_line_width

bool VPlug_GeoShapes::setParam(string param, string value)
{
  if(strContains(param == "_viewable_"))
    return(setViewableMapping(param, value));
  else if(strContains(param, "_color"))
    return(setColorMapping(param, value));
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setViewableMapping

bool VPlug_GeoShapes::setViewableMapping(string param, string value)
{
  param = tolower(param);
  value = tolower(value);

  if((value == "true") or (value == "on")) {
    m_viewable_map[param] = true;
    return(true);
  }
  if((value == "false") or (value == "off")) {
    m_viewable_map[param] = false;
    return(true);
  }
  if(value != "toggle")
    return(false);
  else {
    map<string, vector<double> >::iterator p;
    p = m_viewable_map.find(attribute);
    if(p == m_color_map.end())
      m_viewable_map[param] = true;
    else {
      book curr_value = p->second;
      m_viewable_map[param] = !curr_value;
    }
  }
  return(true);
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label", "DarkKhaki"
//            "label"  " hex, bd, b7, 6b"
//            "label", "0.741, 0.718, 0.420"

bool VPlug_GeoShapes::setColorMapping(string attribute, string color_str)
{
  attribute = stripBlankEnds(attribute);
  color_str = stripBlankEnds(color_str);
  
  vector<double> cvect = colorParse(color_str);
  
  m_color_map[attribute] = cvect;
  if((cvect[0]==0) && (cvect[2]==0) && (cvect[2]==0) &&
     (tolower(color_str) != "black"))
    return(false);
  return(true);    
}

//-------------------------------------------------------------
// Procedure: setColorMapping
//            "label, DarkKhaki"
//            "label, hex, bd, b7, 6b"
//            "label, 0.741, 0.718, 0.420"

bool VPlug_GeoShapes::setColorMapping(string str)
{
  string attribute = biteString(str, ',');
  return(setColorMapping(attribute, str));
}

//-------------------------------------------------------------
// Procedure: getColorMapping
//      Note: If the attribute is not found, the returned color
//            vector can be determined by the optional def_color
//            argument. 

vector<double> VPlug_GeoShapes::getColorMapping(string attribute, 
						string def_color)
{
  map<string, vector<double> >::iterator p;
  p = m_color_map.find(attribute);
  if(p != m_color_map.end())
    return(p->second);
  else {
    vector<double> color_vector = colorParse(def_color);
    return(color_vector);
  }
}


#if 0

//-----------------------------------------------------------
// Procedure: setColorParam

bool VPlug_GeoShapes::setColorParam(vector<double>& cvect,
				    const string& color_str)
{
  vector<double> cv = colorParse(color_str);
  if((cv[0]==0) && (cv[1]==0) && (cv[2]==0) && (tolower(color_str)!="black"))
    return(false);
  else {
    cvect = cv;
    return(true);
  }
}

//-----------------------------------------------------------
// Procedure: setParam()

bool VPlug_GeoShapes::setParam(string param, double value)
{
  if(param == "mod_scale_all") {
    if((m_marker_scale_global * value) > 0.1)
      m_marker_scale_global *= value;
  }
  else if(param == "set_scale_all") {
    if(value > 0.1)
      m_marker_scale_global = value;
  }
  else
    return(false);
  return(true);
}

//-----------------------------------------------------------
// Procedure: viewable

bool VPlug_GeoShapes::viewable(const string& str)
{
  if((str == "all") || (tolower(str) == "all"))
    return(m_marker_viewable_all);
  else if((str == "labels") || (tolower(str) == "labels"))
    return(m_marker_viewable_labels);
  return(false);
}

//-----------------------------------------------------------
// Procedure: getMarkerType()

string VPlug_GeoShapes::getMarkerType(int ix)
{
  if((ix >= 0) && (ix < m_marker_type.size()))
    return(m_marker_type[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerXPos()

double VPlug_GeoShapes::getMarkerXPos(int ix)
{
  if((ix >= 0) && (ix < m_marker_xpos.size()))
    return(m_marker_xpos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerYPos()

double VPlug_GeoShapes::getMarkerYPos(int ix)
{
  if((ix >= 0) && (ix < m_marker_ypos.size()))
    return(m_marker_ypos[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerScale()

double VPlug_GeoShapes::getMarkerScale(int ix)
{
  if((ix >= 0) && (ix < m_marker_scale.size()))
    return(m_marker_scale[ix]);
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getMarkerLabel()

string VPlug_GeoShapes::getMarkerLabel(int ix)
{
  if((ix >= 0) && (ix < m_marker_label.size()))
    return(m_marker_label[ix]);
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: getMarkerColorString

string VPlug_GeoShapes::getMarkerColorString(int mix, int cix)
{
  if((mix < 0) && (mix >= m_marker_colors.size()))
    return("");
  
  if((cix < 0) && (cix >= m_marker_colors[mix].size()))
    return("");
  
  return(m_marker_colors[mix][cix]);
}


//-----------------------------------------------------------
// Procedure: getMarkerColorVector

vector<vector<double> > VPlug_GeoShapes::getMarkerColorVectors(int mix)
{
  vector<vector<double> > fail_vector;

  if((mix < 0) && (mix >= m_marker_color_vectors.size()))
    return(fail_vector);
  else
    return(m_marker_color_vectors[mix]);
}
#endif
