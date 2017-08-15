/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT                                             */
/*    FILE: LoiterAssign.cpp                                */
/*    DATE: August 9th, 2017                                */
/************************************************************/

#include <iterator>
#include <algorithm>
#include "MBUtils.h"
#include "ACTable.h"
#include "LoiterAssign.h"
#include "XYFormatUtilsPoly.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LoiterAssign::LoiterAssign()
{
  // Initialize config variables
  m_derange  = true;
  m_poly_rad = 20;
  m_interval = 90;
  m_moosvar_assign = "UP_LOITER";

  m_opreg_poly_edge_color = "yellow";
  m_opreg_poly_vert_color = "green";

  m_loiter_poly_edge_color = "white";
  m_loiter_poly_vert_color = "dodger_blue";

  // Initialize state variables
  m_total_reassigns = 0;
  m_time_prev_assign = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool LoiterAssign::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool LoiterAssign::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool LoiterAssign::Iterate()
{
  AppCastingMOOSApp::Iterate();

  conditionallyReassign();

  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool LoiterAssign::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "poly_rad") 
      handled = setNonNegDoubleOnString(m_poly_rad, value);
    else if(param == "poly_sep") 
      handled = setNonNegDoubleOnString(m_poly_sep, value);
    else if(param == "interval") 
      handled = setNonNegDoubleOnString(m_interval, value);
    else if(param == "derange") 
      handled = setBooleanOnString(m_derange, value);
    else if((param == "vname") || (param == "vnames")) 
      handled = handleConfigVNames(value);
    else if(param == "loiterpoly") 
      handled = handleConfigLoiterPoly(value);
    else if(param == "opregion") 
      handled = handleConfigOpRegion(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  handleFinalConfiguration();
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void LoiterAssign::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}

//---------------------------------------------------------
// Procedure: handleConfigOpRegion

bool LoiterAssign::handleConfigOpRegion(string opstr)
{
  XYPolygon opregion = string2Poly(opstr);
  opregion.set_label("la_opregion");

  if(!opregion.is_convex())
    return(false);

  m_opregion = opregion;
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigLoiterPoly

bool LoiterAssign::handleConfigLoiterPoly(string polystr)
{
  XYPolygon poly = string2Poly(polystr);

  if(!poly.is_convex())
    return(false);

  if(poly.get_label() == "") {
    string default_label = "default_";
    default_label += uintToString(m_polys.size()+1);
    poly.set_label(default_label);
  }
				   
  m_polys.push_back(poly);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigVNames()

bool LoiterAssign::handleConfigVNames(string vnames)
{
  bool no_dupl_found = true;
  
  vnames = stripBlankEnds(vnames);
  vector<string> svector = parseString(vnames, ':');
  for(unsigned int i=0; i<svector.size(); i++) {
    string vname = stripBlankEnds(svector[i]);
    if(vectorContains(m_vnames, vname))
      no_dupl_found = false;
    else
      m_vnames.push_back(vname);
  }

  return(no_dupl_found);
}


//---------------------------------------------------------
// Procedure: handleFinalConfiguration()

void LoiterAssign::handleFinalConfiguration()
{
  if(!m_opregion.is_convex()) {
    reportUnhandledConfigWarning("Bad or missing OpRegion");
    return;
  }
  m_opregion.set_color("edge", m_opreg_poly_edge_color);
  m_opregion.set_color("vertex", m_opreg_poly_vert_color);
  
    
  // Part 1: Make sure each given fixed polys are within opregion
  for(unsigned int i=0; i<m_polys.size(); i++) {
    if(!polyWithinOpRegion(m_polys[i])) {
      string warning = m_polys[i].get_label() + " poly not in opregion";
      reportUnhandledConfigWarning(warning);
    }
    m_polys[i].set_color("edge", m_loiter_poly_edge_color);
    m_polys[i].set_color("vertex", m_loiter_poly_vert_color);
  }
  
  // Part 2: Make sure each of the given fixed polys are within
  // opregion by the amount specified by poly_sep
}


//---------------------------------------------------------
// Procedure: conditionallyReassign()

void LoiterAssign::conditionallyReassign()
{
  double curr_time = MOOSTime();

  double elapsed_time = curr_time - m_time_prev_assign;

  if((elapsed_time < m_interval) && (m_time_prev_assign != 0))
    return;

  
  if(m_derange)
    derangeReassign();
  else
    rebuildReassign();

  postViewablePolys();

  m_total_reassigns++;
  m_time_prev_assign = curr_time;

  for(unsigned int i=0; i<m_vnames.size(); i++)
    Notify("LA_NEW_ASSIGN_" + toupper(m_vnames[i]), "true");
}


//---------------------------------------------------------
// Procedure: derangeReassign()

void LoiterAssign::derangeReassign()
{
  bool done = false;

  unsigned int fixpoint_checks = 0;
  unsigned int max_fixpoint_checks = 100;

  vector<string> vnames = m_vnames;
  
  while(!done && (fixpoint_checks < max_fixpoint_checks)) {
    random_shuffle(vnames.begin(), vnames.end());
    // Now check for fixpoints. if one is found, done is false
    done = true;
    for(unsigned int i=0; i<vnames.size(); i++) 
      if(vnames[i] == m_vnames[i])
	done = false;
    fixpoint_checks++;
  }

  string str;
  for(unsigned int i=0; i<m_vnames.size(); i++) {
    if(i!=0)
      str += ",";
    str += m_vnames[i];
  }
  Notify("LA_PERMUTATION", str);
  
  m_vnames = vnames;
}


//---------------------------------------------------------
// Procedure: rebuildReassign()

void LoiterAssign::rebuildReassign()
{
  vector<XYPolygon> new_polys;

  for(unsigned int i=0; i<m_vnames.size(); i++) {
    cout << "Index: " << i << endl;
    double px = 0;
    double py = 0;
    bool done = false;
    while(!done) {
      bool ok = randPointInPoly(m_opregion, px, py, 1000);
      if(ok) {
	string str = "format=radial";
	str += ",x=" + doubleToString(px,2);
	str += ",y=" + doubleToString(py,2);
	str += ",radius=" + doubleToString(m_poly_rad,2);
	str += ",pts=8, snap=1";
	XYPolygon try_poly = string2Poly(str);
	if(polyWithinOpRegion(try_poly)) {
	  bool overlap=false;
	  for(unsigned int j=0; j<i; j++)
	    //if(try_poly.intersects(new_polys[j]))
	    if(try_poly.dist_to_poly(new_polys[j]) < 10)
	      overlap = true;
	  if(!overlap) {
	    new_polys.push_back(try_poly);
	    done = true;
	  }
	}
      }
    }
  }
  
  m_polys = new_polys;
  
}


//---------------------------------------------------------
// Procedure: polyWithinOpRegion

bool LoiterAssign::polyWithinOpRegion(XYPolygon poly)
{
  if(!m_opregion.is_convex())
    return(false);

  if(!m_opregion.contains(poly))
    return(false);

  return(true);  
}


//---------------------------------------------------------
// Procedure: postViewablePolys

void LoiterAssign::postViewablePolys()
{
  string opregion_poly_str = m_opregion.get_spec();
  Notify("VIEW_POLYGON", opregion_poly_str);

  for(unsigned int i=0; i<m_vnames.size(); i++) {
    string mvar = m_moosvar_assign;
    if(!strEnds(mvar, "_"))
      mvar += "_";
    mvar += toupper(m_vnames[i]);

    m_polys[i].set_label("loiter_" + tolower(m_vnames[i]));
    Notify(mvar, "polygon = " + m_polys[i].get_spec());
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool LoiterAssign::buildReport() 
{
  m_msgs << "Configuration:                               \n";
  m_msgs << "============================================ \n";
  m_msgs << "Number of vehicles: " << m_vnames.size() << endl;
  m_msgs << "derange mode:       " << boolToString(m_derange) << endl;

  for(unsigned int i=0; i<m_vnames.size(); i++)
    m_msgs << "  " << m_vnames[i] << endl;

  m_msgs << "Polygons:                                    \n";
  m_msgs << "============================================ \n";
  m_msgs << "Number of loiter polys: " << m_polys.size() << endl;

  for(unsigned int i=0; i<m_polys.size(); i++)
    m_msgs << "  " << m_polys[i].get_label() << endl;


  m_msgs << "Polygon Assignment:                          \n";
  m_msgs << "============================================ \n";
  m_msgs << "Total Reassigns: " << m_total_reassigns << endl;

  for(unsigned int i=0; i<m_vnames.size(); i++) {
    string polyname = "unassigned";
    if(i < m_polys.size())
      polyname = m_polys[i].get_label();
    m_msgs << "[" << m_vnames[i] << "]: " << polyname << endl; 

  }
  
  return(true);
}




