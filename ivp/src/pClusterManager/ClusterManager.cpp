/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ClusterManager.cpp                                   */
/*    DATE: May 27th 2007                                        */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <math.h>
#include <iterator>
#include "MBUtils.h"
#include "ClusterManager.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

ClusterManager::ClusterManager()
{
  m_migrate_active  = false;
  m_verbosity       = "verbose";
  m_skews_matter    = true;
  m_ok_skew         = 300;   // seconds - 5mins

  m_cmg_x      = 0;
  m_cmg_y      = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool ClusterManager::OnNewMail(MOOSMSG_LIST &NewMail)
{
  double curr_time = MOOSTime() - GetAppStartTime();

  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.m_sKey;
    string sdata = tolower(msg.m_sVal);
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;
    double dfT;
    msg.IsSkewed(MOOSTime(),&dfT);
    
    if(!m_skews_matter || (fabs(dfT) < m_ok_skew)) {
      if(key == "CMG_ACTIVATE") {
	if(MOOSStrCmp(sdata, "true"))
	  m_migrate_active = true;
	else
	  m_migrate_active = false;
      }
      else if(key == "CMG_VERBOSITY") {
	if((sdata=="verbose")||(sdata=="quiet")||(sdata=="terse"))
	  m_verbosity = sdata;
      }
      else if(key == "CMG_TRAJECTORY") {
	XYSegList new_seglist;
	bool ok = new_seglist.initialize(sdata);
	if(ok)
	  m_migrator.setSegList(new_seglist);
      }
    }
    else {
      string warning_str = "\n pClusterManager ignores MOOS msg [";
      warning_str += key;
      warning_str += "] due to skew";
      MOOSTrace(warning_str);
      MOOSDebugWrite(warning_str);
    }
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool ClusterManager::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  
  registerVariables();

  return(true);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool ClusterManager::OnStartUp()
{
  MOOSTrace("pClusterManager starting....\n");

  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "VERBOSITY")) {
      sLine = tolower(sLine);
      if((sLine!="verbose")&&(sLine!="terse")&&(sLine!="quiet")) {
	MOOSTrace("Improper VERBOSE value: %s\n", sLine.c_str());
	return(false);
      }
      m_verbosity = sLine;
    }
    if(MOOSStrCmp(sVarName, "POSITION_X")) {
      if(!isNumber(sLine)) {
	MOOSTrace("Improper POSITION_X value: %s\n", sLine.c_str());
	return(false);
      }
      m_cmg_x = atof(sLine.c_str());
    }
    if(MOOSStrCmp(sVarName, "POSITION_Y")) {
      if(!isNumber(sLine)) {
	MOOSTrace("Improper POSITION_Y value: %s\n", sLine.c_str());
	return(false);
      }
      m_cmg_y = atof(sLine.c_str());
    }
  }
    
  registerVariables();

  return(true);
}


//--------------------------------------------------------------------
// Procedure: Iterate()
//      Note: happens AppTick times per second
//            

bool ClusterManager::Iterate()
{
  postCharStatus();
  
  double curr_time = MOOSTime() - GetAppStartTime();

  if(m_migrate_active) {
    m_migrator.updatePosition(curr_time);
    m_cmg_x = m_migrator.getCurrX();
    m_cmg_y = m_migrator.getCurrY();
  }

  m_Comms.Notify("CMG_POSITION_X", m_cmg_x);
  m_Comms.Notify("CMG_POSITION_Y", m_cmg_y);
  
  return(true);
}
  

//------------------------------------------------------------
// Procedure: postCharStatus()

void ClusterManager::postCharStatus()
{
  if(m_verbosity == "silent")
    return;

  if(m_migrate_active) {
    if(m_verbosity == "terse")
      MOOSTrace("$");
  }
  else
    MOOSTrace("*");
}

//------------------------------------------------------------
// Procedure: registerVariables

void ClusterManager::registerVariables()
{
  m_Comms.Register("CMG_VERBOSE", 0);
  m_Comms.Register("CMG_ACTIVATE", 0);
  m_Comms.Register("CMG_TRAJECTORY", 0);
}


//------------------------------------------------------------
// Procedure: alterMigrateState

void ClusterManager::alterMigrateState(bool g_migrate_active)
{
  if(m_migrate_active == g_migrate_active)
    return;

  double curr_time = MOOSTime() - GetAppStartTime();
  
  if(!m_migrate_active) {
    m_migrate_active = true;
    m_migrator.setInitialConditions(curr_time, m_cmg_x, m_cmg_y);
  }
  else
    m_migrate_active = false;
}
