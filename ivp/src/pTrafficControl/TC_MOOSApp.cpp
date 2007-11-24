/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TC_MOOSApp.cpp                                       */
/*    DATE: July 17th 2007                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include "TC_MOOSApp.h"
#include "MBUtils.h"
#include "TrajectoryGenerator.h"
#include "PopulatorTSet.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

TC_MOOSApp::TC_MOOSApp()
{
  m_last_reset_time   = MOOSTime();
  m_wait_period       = 0;
  m_reset_request     = false;
  m_paused            = false;
  m_first_iter        = true;
  m_paused_time_total = 0;
  m_paused_time_start = 0;
  m_vtset_ix          = 0;
  m_tset_count        = 0;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool TC_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    if(key == "TC_RESET") {
      m_reset_request = true;
    }
    if(key == "TC_RESET_PAUSE") {
      if(tolower(sdata) == "true") {
	if(!m_paused) {
	  m_paused = true;
	  m_paused_time_start = MOOSTime();
	}
      }
      else {
	m_paused = false;
	m_paused_time_total = MOOSTime() - m_paused_time_start;
      }
    }
      
    else {
      MOOSTrace("TC_MOOSApp: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
    }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool TC_MOOSApp::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
 
  m_Comms.Register("TC_RESET", 0);
  m_Comms.Register("TC_RESET_PAUSE", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool TC_MOOSApp::Iterate()
{
  double curr_time = MOOSTime();
  double elapsed_time = curr_time - m_last_reset_time;
  double paused_time;

  if(m_paused)
    m_paused_time_total = (curr_time = m_paused_time_start);

  elapsed_time = elapsed_time - m_paused_time_total;

  cout << "Elapsed_time: " << elapsed_time << endl;
  cout << "Wait_period: "  << m_wait_period << endl << endl;
  
  if(m_reset_request || m_first_iter ||
     ((m_wait_period > 0) && (elapsed_time > m_wait_period))) {
    
    makeTSet();
    postTSet();

    m_last_reset_time = curr_time;
    m_reset_request = false;
    m_paused_time_total = 0;
  }
  
  m_Comms.Notify("TSET_COUNT", m_tset_count);
  
  m_first_iter = false;
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool TC_MOOSApp::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("pTC_MOOSApparting....\n");

  bool ok; 
  bool seed = true;

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {
      string sLine    = *p;
      string sVarName = toupper(MOOSChomp(sLine, "="));
      sLine = tolower(stripBlankEnds(sLine));

      if(MOOSStrCmp(sVarName, "WAIT_PERIOD")) {
	sLine = stripBlankEnds(sLine);
	m_wait_period = atof(sLine.c_str());
      }
      if(sVarName == "VIDS")
	ok = handleConfig("vids", sLine);
      if(sVarName == "CENTER")
	ok = handleConfig("center", sLine);
      if(sVarName == "RADIUS")
	ok = handleConfig("radius", sLine);
      if(sVarName == "VARIANCE")
	ok = handleConfig("variance", sLine);
      if(sVarName == "SPEED")
	ok = handleConfig("speed", sLine);
      if(sVarName == "FILE")
	ok = handleConfig("file", sLine);
      if(sVarName == "NOSEED")
	seed = (sLine == "true");

      if(!ok) {
	cout << "Problem with parameter: " << sVarName << endl;
	return(false);
      }
    }
  }

  if(seed) {
    unsigned long tseed = time(NULL);
    unsigned long hostid = gethostid();
    unsigned long pid = (long)getpid();
    unsigned long seed = (tseed%999999);
    seed = ((rand())*seed*hostid)%999999;
    seed = (seed*pid)%999999;
    srand(seed);
  }
  
  //cout << "Vehicles: " << endl;
  //for(int i=0; i<m_vehicles.size(); i++)
  //  cout << i << ": " << m_vehicles[i] << endl;


  return(true);
}

//-----------------------------------------------------------------
// Procedure: makeTSet
//      Note: 

void TC_MOOSApp::makeTSet()
{
  
  if(m_vtset.size() != 0) {
    if(m_vtset_ix < m_vtset.size()) {
      m_trajectory_set = m_vtset[m_vtset_ix];
      m_vtset_ix++;
      m_tset_count++;
    }
  }
  else {
    TrajectoryGenerator tgen;
    tgen.generate(m_center_x, m_center_y, m_radius, m_variance, 
		  m_vehicles.size(), m_speed);
    m_trajectory_set = tgen.getTSet();
    m_tset_count++;
  }
}


//-----------------------------------------------------------------
// Procedure: postTSet
//      Note: 

void TC_MOOSApp::postTSet()
{
  int vsize = m_trajectory_set.size();

  if(vsize < 2) {
    cout << "Request to post TSet size=" << vsize << " ignored." << endl;
    return;
  }

  for(int i=0; i<vsize; i++) {
    string var, val;
    string x   = doubleToString(m_trajectory_set.getStartX(i), 1);
    string y   = doubleToString(m_trajectory_set.getStartY(i), 1);
    string spd = doubleToString(m_trajectory_set.getStartSPD(i), 1);
    string hdg = doubleToString(m_trajectory_set.getStartHDG(i), 1);
    
    var = "MARINESIM_RESET_" + toupper(m_vehicles[i]);

    val  = "start_x=" + x +", "; 
    val += "start_y=" + y +", "; 
    val += "start_speed=" + spd +", "; 
    val += "start_heading=" + hdg;
    m_Comms.Notify(var, val);

    
    x = doubleToString(m_trajectory_set.getDestX(i), 1);
    y = doubleToString(m_trajectory_set.getDestY(i), 1);
    var = "UPDATES_WPT_" + toupper(m_vehicles[i]);
    val = "points=" + x + "," + y + " # speed=" + spd;
    m_Comms.Notify(var, val);
  }

  m_Comms.Notify("STATION_KEEP_ALL", "false");
  m_Comms.Notify("TRAIL_RESET", "n/a");
}


//-----------------------------------------------------------------
// Procedure: handleConfig
//      Note: 

bool TC_MOOSApp::handleConfig(const string& g_param, 
			      const string& g_value)
{
  string param_value = stripBlankEnds(g_value);

  string param = tolower(g_param);
  if(param == "vids") {
    vector<string> svector = parseString(param_value, ',');
    int vsize = svector.size();
    for(int i=0; i<vsize; i++)
      m_vehicles.push_back(stripBlankEnds(svector[i]));
    return(true);
  }

  if(param == "center") {
    vector<string> svector = parseString(param_value, ',');
    if(svector.size() == 2) {
      svector[0] = stripBlankEnds(svector[0]);
      svector[1] = stripBlankEnds(svector[1]);
      if(!isNumber(svector[0]) || !isNumber(svector[1]))
	return(false);
      m_center_x = atof(svector[0].c_str());
      m_center_y = atof(svector[1].c_str());
    }
    return(true);
  }

  if(param == "radius") {
    if(!isNumber(param_value))
      return(false);
    m_radius = atof(param_value.c_str());
    return(true);
  }

  if(param == "variance") {
    if(!isNumber(param_value))
      return(false);
    m_variance = atof(param_value.c_str());
    return(true);
  }

  if(param == "speed") {
    if(!isNumber(param_value))
      return(false);
    m_speed = atof(param_value.c_str());
    return(true);
  }
  
  if(param == "file") {
    PopulatorTSet populator;
    m_vtset = populator.populate(g_value);
    if(m_vtset.size() == 0)
      return(false);
    return(true);
  }
}
