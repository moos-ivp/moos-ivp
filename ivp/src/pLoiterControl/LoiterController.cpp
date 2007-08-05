/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LoiterController.cpp                                 */
/*    DATE: Feb 9th 2006                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include "LoiterController.h"
#include "MBUtils.h"
#include "Permute.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

LoiterController::LoiterController()
{
  last_mapping_time   = MOOSTime();
  wait_period         = 0;
  mapping_request     = false;
  m_paused            = false;
  m_paused_time_total = 0;
  m_paused_time_start = 0;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool LoiterController::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key = msg.m_sKey;
    string community = msg.m_sOriginatingCommunity;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    if(key == "LOITER_REMAP") {
      mapping_request = true;
    }
    if(key == "LCONTROL_PAUSE") {
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
      MOOSTrace("LoiterController: Unknown msg [%s]\n",msg.m_sKey.c_str());
    }
    }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool LoiterController::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
 
  m_Comms.Register("LOITER_REMAP", 0);
  m_Comms.Register("LCONTROL_PAUSE", 0);
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool LoiterController::Iterate()
{
  double curr_time = MOOSTime();
  double elapsed_time = curr_time - last_mapping_time;
  double paused_time;

  if(m_paused)
    m_paused_time_total = (curr_time = m_paused_time_start);

  elapsed_time = elapsed_time - m_paused_time_total;

  cout << "Elapsed_time: " << elapsed_time << endl;
  cout << "Wait_period: " << wait_period << endl << endl;
  
  if(mapping_request || 
     ((wait_period > 0) && (elapsed_time > wait_period))) {
    curr_mapping = permute(curr_mapping, 10);
    postMapping();
    last_mapping_time = curr_time;
    mapping_request = false;
    m_paused_time_total = 0;
  }
  
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool LoiterController::OnStartUp()
{
  CMOOSApp::OnStartUp();

  MOOSTrace("pLoiterControl starting....\n");

  bool seed = true;

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {
      string sLine    = *p;
      string sVarName = MOOSChomp(sLine, "=");

      if(MOOSStrCmp(sVarName, "WAIT_PERIOD")) {
	sLine = stripBlankEnds(sLine);
	wait_period = atof(sLine.c_str());
      }
      if(MOOSStrCmp(sVarName, "NOSEED")) {
	sLine = tolower(stripBlankEnds(sLine));
	if(sLine == "true")
	  seed = false;
      }
      else if(MOOSStrCmp(sVarName, "ASSIGNMENT")) {
	sLine = stripBlankEnds(sLine);
	vector<string> svector = chompString(sLine, '|');
	if(svector.size() != 2)
	  return(false);
	string vehicle = stripBlankEnds(svector[0]);
	string region  = stripBlankEnds(svector[1]);

	if((vehicle == "") || (region == ""))
	  return(false);

	// Vehicles and Regions must be unique
	int vsize = vehicles.size();
	for(int i=0; i<vsize; i++) {
	  if(vehicle == vehicles[i])
	    return(false);
	  if(region  == regions[i])
	    return(false);
	}

	vehicles.push_back(vehicle);
	regions.push_back(region); 
	curr_mapping.push_back(vsize);
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

  return(true);
}

//-----------------------------------------------------------------
// Procedure: postMapping
//      Note: 
//
//  "LOITER_UPDATE_Kayak_01", "polygon=radial:10,10,50,12"

bool LoiterController::postMapping()
{
  int vsize = vehicles.size();

  cout << "New Mapping:  " << endl;
  for(int i=0; i<vsize; i++) {
    string vehicle = vehicles[i];
    string region  = regions[curr_mapping[i]];
    string str = "LOITER_UPDATE_" + vehicle;
    string val = "polygon=" + region;
    m_Comms.Notify(str, val);
    cout << "  " << vehicle << ", " << region << endl;
  }
  return(true);
}




