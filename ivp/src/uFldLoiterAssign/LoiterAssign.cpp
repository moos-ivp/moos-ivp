/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT                                             */
/*    FILE: LoiterAssign.cpp                                */
/*    DATE: August 9th, 2017                                */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "LoiterAssign.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LoiterAssign::LoiterAssign()
{
}

//---------------------------------------------------------
// Destructor

LoiterAssign::~LoiterAssign()
{
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

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "poly_rad") 
      handled = setNonNegDoubleOnString(m_poly_rad, value);
    else if(param == "poly_sep") 
      handled = setNonNegDoubleOnString(m_poly_sep, value);
    else if((param == "vname") || (param == "vnames")) 
      handled = handleConfigVNames(value);
    else if(param == "loiterpoly") 
      handled = handleConfigLoiterPoly(value);
    else if(param == "opregion") 
      handled = handleConfigOpRegion(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  checkFinalConfiguration();
  
    
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

  m_polys.push_back(poly);
  return(true);
}

//---------------------------------------------------------
// Procedure: handleConfigVNames()

bool LoiterAssign::handleConfigVNames(string vnames)
{
  bool no_dupl_found = true;
  
  vnames = stripBlankEnds(vnames);
  vector<string> svector = parseString(vnames, ',');
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
// Procedure: checkFinalConfiguration()

void LoiterAssign::checkFinalConfiguration()
{
  // Part 1: Make sure each given fixed polys are within opregion
  for(unsigned int i=0; i<m_polys.size(); i++) {
    if(!polyWithinOpRegion(m_polys[i])) {
      string warning = m_polys[i].get_label() + " poly not in opregion";
      reportUnhandledConfigWarning(warning);
    }
  }
  
  // Part 2: Make sure each of the given fixed polys are within
  // opregion by the amount specified by poly_sep
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


//------------------------------------------------------------
// Procedure: buildReport()

bool LoiterAssign::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




