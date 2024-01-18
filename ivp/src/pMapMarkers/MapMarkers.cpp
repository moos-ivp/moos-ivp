/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MapMarkers.cpp                                  */
/*    DATE: September 25th, 2023                            */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "MapMarkers.h"
#include "Populator_OpField.h"
#include "XYFormatUtilsMarker.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

MapMarkers::MapMarkers()
{
  // Config vars
  m_marker_size  = 3;
  m_show_markers = false;
  m_show_mlabels = true;
  m_marker_color = "light_green";
  m_marker_shape = "circle";
  m_marker_lcolor = "gray60";
  m_marker_ecolor = "black";
  
  // State vars
  m_post_markers_utc = 0;
  m_refresh_needed = true;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool MapMarkers::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key  = msg.GetKey();
    string sval = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = false;
    if(key == "MAP_MARKERS") {
      bool prev_show_markers = m_show_markers;	
      handled = setBooleanOnString(m_show_markers, sval);
      if(prev_show_markers != m_show_markers)
	m_post_markers_utc = 0;
    }
    
    else if(key == "APPCAST_REQ")
      handled = true;
    
    else if(!handled) 
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool MapMarkers::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool MapMarkers::Iterate()
{
  AppCastingMOOSApp::Iterate();

  postMarkers();

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool MapMarkers::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "opfield")
      handled = m_opfield.config(value);
    else if(param == "show_markers") 
      handled = setBooleanOnString(m_show_markers, value);
    else if(param == "show_mlabels") 
      handled = setBooleanOnString(m_show_mlabels, value);

    else if(param == "marker_color") 
      handled = setColorOnString(m_marker_color, value);
    else if(param == "marker_lcolor") 
      handled = setColorOnString(m_marker_lcolor, value);
    else if(param == "marker_ecolor") 
      handled = setColorOnString(m_marker_ecolor, value);
    else if(param == "marker_key") 
      handled = setNonWhiteVarOnString(m_map_marker_key, value);
    else if(param == "marker_shape") 
      handled = handleConfigMarkerShape(value);
    else if(param == "marker_size") 
      handled = setUIntOnString(m_marker_size, value);

    else if(param == "opfile") 
      handled = handleConfigFileOPF(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }

  buildOpFieldFromFiles();
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void MapMarkers::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("MAP_MARKERS", 0);
}

//---------------------------------------------------------
// Procedure: handleConfigFileOPF()
 
bool MapMarkers::handleConfigFileOPF(string str)
{
  if(vectorContains(m_opf_files, str)) {
    reportConfigWarning("Duplicate opf file: " + str);
    return(false);
  }

  m_opf_files.push_back(str);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigMarkerShape()
 
bool MapMarkers::handleConfigMarkerShape(string shape)
{
  if((shape == "circle")  || (shape == "triangle") ||
     (shape == "square")  || (shape == "diamond")  ||
     (shape == "gateway") || (shape == "efield")) {
    m_marker_shape = shape;
    return(true);
  }
  
  return(false);
}

//---------------------------------------------------------
// Procedure: buildOpFieldFromFiles()

bool MapMarkers::buildOpFieldFromFiles()
{
  // Case of no files is fine, just return.
  if(m_opf_files.size() == 0)
    return(true);

  // Part 1: Handle each file
  Populator_OpField populator;
  for(unsigned int i=0; i<m_opf_files.size(); i++) {
    string filename = m_opf_files[i];
    cout << "filename:" << filename << endl;
    bool ok = populator.addFileOPF(filename);
    if(!ok) {
      string msg = "Err in naming opf file:" + filename; 
      reportConfigWarning(msg);
    }
    
    if(ok) 
      ok = populator.populate();

    if(!ok) {
      string msg = "Err in loading opf file:" + filename; 
      reportConfigWarning(msg);
      return(false);
    }
  }

  // Part 2: If no resulting elements, this is an error.
  OpField opfield = populator.getOpField();
  if(opfield.size() == 0)
    return(false);
  
  // Part 3: Handle the case were the opfield is comprised
  // of some 0elements loaded from file(s), and some
  // elements set explicitly in the config file. If there
  // are already elements in the opfield prior to loading
  // from files, the merge them.
  if(m_opfield.size() != 0) {
    bool ok = m_opfield.merge(opfield);
    if(!ok)
      reportConfigWarning("Err merging opfields");
  }
  else
    m_opfield = opfield;

  m_opfield.simplify();
  
  return(true);
}

//---------------------------------------------------------
// Procedure: addMarker()

bool MapMarkers::addMarker(string str)
{
  XYMarker marker = string2Marker(str);
  if(!marker.is_set_x() || !marker.is_set_y())
    return(false);
  
  string label = marker.get_label();
  if(label == "") 
    label = "m_" + uintToString(m_map_markers.size());

  m_map_markers[label] = marker;
  return(true);
}

//---------------------------------------------------------
// Procedure: postMarkers()

void MapMarkers::postMarkers()
{
  double elapsed = m_curr_time - m_post_markers_utc;
  if(m_iteration < 100) {
    if(elapsed < 5)
      return;
  }
  else if(!m_refresh_needed)
    return;
  
  vector<string> aliases = m_opfield.getPtAliases();
  for(unsigned int i=0; i<aliases.size(); i++) {
    string alias = aliases[i];

    string spec;
    if(!m_show_markers)
      spec = "x=0,y=0,active=false,label=" + alias;
    else {
      XYPoint pt = m_opfield.getPoint(alias);
      XYMarker marker;
      marker.set_vx(pt.x());
      marker.set_vy(pt.y());
      marker.set_label(alias);
      marker.set_label_color(m_marker_lcolor);
      marker.set_type(m_marker_shape);
      string color = m_opfield.getColor(alias);
      if(color == "")
	color = m_marker_color;
      marker.set_color("primary_color", color);
      if(m_marker_ecolor != "black")
	marker.set_edge_color(m_marker_ecolor);
      marker.set_width(m_marker_size);
      spec = marker.get_spec();
    }
    Notify("VIEW_MARKER", spec);
  }
	
  m_post_markers_utc = m_curr_time;  
  m_refresh_needed = false;
}


//------------------------------------------------------------
// Procedure: buildReport()

bool MapMarkers::buildReport() 
{
  m_msgs << "Points: " << uintToString(m_opfield.size()) << endl;
  m_msgs << "Show Markers: " << boolToString(m_show_markers) << endl;
  m_msgs << "Default Color: " << m_marker_color << endl;
  m_msgs << "Default Size: " << m_marker_size << endl;
  m_msgs << "Marker Key: " << m_map_marker_key << endl;
  m_msgs << endl;

  return(true);
  ACTable actab(4);
  actab << "Alias | Point | Alias | Point";
  actab.addHeaderLines();

  vector<string> aliases = m_opfield.getPtAliases();
  for(unsigned int i=0; i<aliases.size(); i++) {
    string alias = aliases[i];
    XYPoint pt = m_opfield.getPoint(alias);
    actab << alias << pt.get_spec();
  }

  m_msgs << actab.getFormattedString();

  return(true);
}




