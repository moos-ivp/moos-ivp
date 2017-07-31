/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LoadWatch.cpp                                        */
/*    DATE: Dec 24th, 2013                                       */
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
#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "LoadWatch.h"

using namespace std;

//---------------------------------------------------------
// Constructor

LoadWatch::LoadWatch()
{
  m_breach_trigger = 1; // First offense forgiven, 2nd offense reported!
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool LoadWatch::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval   = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(strEnds(key, "_ITER_GAP")) 
      handleMailIterGap(key, dval);
    else if(strEnds(key, "_ITER_LEN"))
      handleMailIterLen(key, dval);
    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool LoadWatch::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool LoadWatch::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool LoadWatch::OnStartUp()
{
  string directives  = "must_have_moosblock=false";
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    return(true);

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    // THRESH = app=pHelmIvP, gapthresh=1.5 
    // BREACH_TRIGGER = 1
    if(param == "thresh") {
      handled = handleConfigThresh(value);
    }
    else if((param == "breach_trigger") && isNumber(value)) {
      double dval = atof(value.c_str());
      if(dval < 0)
	dval = 0;
      m_breach_trigger = (unsigned int)(dval);
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void LoadWatch::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("*_ITER_GAP", "*", 0);
  Register("*_ITER_LEN", "*", 0);
}

//---------------------------------------------------------
// Procedure: handleMailIterGap

void LoadWatch::handleMailIterGap(string var, double dval)
{
  string app = findReplace(var, "_ITER_GAP", "");
  
  m_map_app_gap_count[app]++;
  m_map_app_gap_total[app] += dval;

  // If this is the first itergap received for this app, initialize
  // the maximum gap seen so far to zero
  if(m_map_app_gap_max.count(app) == 0)
    m_map_app_gap_max[app] = 0;

  // Compare this itergap to the largest seen so far for this app 
  // and update if this one is larger.
  if(dval > m_map_app_gap_max[app])
    m_map_app_gap_max[app] = dval;  

  bool   thresh_exceeded = false;
  double thresh = 0;

  // Check if the app has threshold name for it explicitly
  if(m_map_thresh.count(app)) {
    thresh = m_map_thresh[app];
    // Check if the app has exceeded that threshold
    if(dval > thresh) 
      thresh_exceeded = true;
  }
  // Otherwise check if there is a generic threshold set for any application.
  else if(m_map_thresh.count("ANY")) {
    thresh = m_map_thresh["ANY"];
    if(dval > thresh) 
      thresh_exceeded = true;
  }
      
  // We're done if the threshold wasn't exceeded in any way.
  if(!thresh_exceeded)
    return;

  if(m_map_app_gap_xcount.count(app)==0)
    m_map_app_gap_xcount[app] = 0;
  m_map_app_gap_xcount[app]++;

  // Check how many times the threshold has been exceeded by this app. 
  // If it does not exceed the trigger amt, then just return
  if(m_map_app_gap_xcount[app] <= m_breach_trigger)
    return;
    
      
  // FINALLY: We have exceeded the threshold for this app and apparently we
  // exceeded it enough times to exceed the trigger amount. So lets act!

  string sval = doubleToString(dval);
  string msg = "Gap Thresh for " + app + " exceeded: " + sval;
  msg += " (" + doubleToStringX(thresh,2) + ")";
  reportRunWarning(msg);
  string warning = "app=" + app +", maxgap=" + sval;
  warning += ", node=" + m_host_community;
  Notify("LOAD_WARNING", warning);

}

//---------------------------------------------------------
// Procedure: handleMailIterLen

void LoadWatch::handleMailIterLen(string var, double dval)
{
  string app = findReplace(var, "_ITER_LEN", "");
  
  m_map_app_len_count[app]++;
  m_map_app_len_total[app] += dval;
  if(m_map_app_len_max.count(app) == 0)
    m_map_app_len_max[app] = 0;
  if(dval > m_map_app_len_max[app])
    m_map_app_len_max[app] = dval;  
}

//---------------------------------------------------------
// Procedure: handleConfigThresh
//   Example:
//            THRESH = app=pHelmIvP, gapthresh=1.5 

bool LoadWatch::handleConfigThresh(string thresh)
{
  string app, gapthresh;

  vector<string> svector = parseString(thresh, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    
    if(param == "app")
      app = toupper(value);
    else if(param == "gapthresh")
      gapthresh = value;
  }

  if((app == "") || (gapthresh == ""))
    return(false);

  double dval = atof(gapthresh.c_str());
  if(dval <= 0)
    return(false);
  
  m_map_thresh[app] = dval;
  return(true);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool LoadWatch::buildReport() 
{
  m_msgs << "Configured Thresholds: " << endl;

  if(m_map_thresh.size() == 0)
    m_msgs << "  None" << endl;
  else {
    map<string,double>::iterator p;
    for(p=m_map_thresh.begin(); p!=m_map_thresh.end(); p++) {
      string app = p->first;
      double thresh = p->second;
      m_msgs << app << ": " << thresh << endl;
    }
  }
  m_msgs << endl;


  ACTable actab(6);

  actab << "Application | AvgGap | MaxGap | AvgLen | MaxLen | Breaches"; 
  actab.addHeaderLines();

  map<string,double>::iterator p;
  for(p=m_map_app_gap_total.begin(); p!=m_map_app_gap_total.end(); p++) {
    string app = p->first;

    double gap_avg = m_map_app_gap_total[app] / (double)(m_map_app_gap_count[app]);
    double gap_max = m_map_app_gap_max[app];

    double len_avg = m_map_app_len_total[app] / (double)(m_map_app_len_count[app]);
    double len_max = m_map_app_len_max[app];
    
    unsigned int breaches = m_map_app_gap_xcount[app];
    
    actab << app << gap_avg << gap_max << len_avg << len_max << breaches;
  }

  m_msgs << actab.getFormattedString();

  return(true);
}








