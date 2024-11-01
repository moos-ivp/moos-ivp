/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: AutoPoke.cpp                                    */
/*    DATE: Oct 30th, 2024                                  */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
//#include "ACTable.h"
#include "AutoPoke.h"
#include "VarDataPairUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

AutoPoke::AutoPoke()
{
  m_required_nodes = 0;
  m_flags_posted = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool AutoPoke::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval  = msg.GetDouble();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "UFSB_NODE_COUNT") 
      m_total_nodes = (unsigned int)(dval);
    
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool AutoPoke::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool AutoPoke::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_flags_posted && (m_total_nodes >= m_required_nodes)) {
    postFlags(m_flags);
    m_flags_posted = true;
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool AutoPoke::OnStartUp()
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
    if(param == "flag") 
      handled = addVarDataPairOnString(m_flags, value);
    else if(param == "required_nodes") 
      handled = setUIntOnString(m_required_nodes, value);
    
    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void AutoPoke::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UFSB_NODE_COUNT", 0);
}

//------------------------------------------------------------
// Procedure: postFlags()

void AutoPoke::postFlags(const vector<VarDataPair>& flags)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();

     // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, later handle macro expansion
    else {
      string sval = pair.get_sdata();
      Notify(moosvar, sval);
    }
  }
}

//------------------------------------------------------------
// Procedure: buildReport()

bool AutoPoke::buildReport() 
{
  m_msgs << "Required Nodes:" << m_required_nodes << endl;
  m_msgs << "Flags: " << endl;
  for(unsigned int i=0; i<m_flags.size(); i++) {
    m_msgs << "[" << i << "]: " << m_flags[i].getPrintable() << endl;
  }
  m_msgs << endl;
  m_msgs << "Node Count:   " << m_total_nodes << endl;
  m_msgs << "flags_posted: " << boolToString(m_flags_posted) << endl;

  return(true);
}




