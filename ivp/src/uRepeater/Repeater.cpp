/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: Repeater.cpp                                    */
/*    DATE: Oct 5th 2007                                    */
/************************************************************/

#include <iostream>
#include <iterator>
#include "Repeater.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool Repeater::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    //double mtime = msg.GetTime();
    string msrc  = msg.GetSource();

    string vtype = "string";
    if(msg.IsDouble())
      vtype = "double";

    cout << "Maybe Refreshing: " << key << endl;
    cout << "Source: " << msrc << endl;
    
    if(msrc != "uRepeater") {
      int ix = findVarIndex(key);
      if(ix != -1) {
	cout << "Refreshing: " << toupper(key) << endl;
	if((m_type[ix]==vtype) || (m_type[ix]=="unset")) {
	  m_count_remaining[ix] = m_rpt_count[ix];
	  cout << "Setting Count to: " << m_rpt_count[ix] << endl;
	  m_time_stamp[ix] = MOOSTime();
	  m_type[ix]  = vtype;
	  m_sVal[ix]  = sval;
	  m_dfVal[ix] = dval;
	}
      }
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Repeater::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  RegisterVariables();
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void Repeater::RegisterVariables()
{
  int vsize = m_rpt_variable.size();
  for(int i=0; i<vsize; i++)
    m_Comms.Register(toupper(m_rpt_variable[i]), 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool Repeater::Iterate()
{
  int vsize = m_rpt_variable.size();
  double curr_time = MOOSTime();

  for(int i=0; i<vsize; i++) {

    //cout << "Considering: " << m_rpt_variable[i] << endl;
    double delta_time = curr_time - m_time_stamp[i];

    //cout << "  DeltaTime: " << delta_time << endl;
    //cout << "  Interval:  " << m_rpt_interval[i] << endl;

    if((delta_time >= m_rpt_interval[i]) && 
       (m_count_remaining[i] > 0)) {
      if(m_type[i] == "string")
	m_Comms.Notify(m_rpt_variable[i], m_sVal[i]);
      else
	m_Comms.Notify(m_rpt_variable[i], m_dfVal[i]);
      m_count_remaining[i]--;
      cout << "Writing: " << m_rpt_variable[i] << 
	":" << m_count_remaining[i] << endl;
      m_time_stamp[i] = curr_time;
    }
  }
		       
		       

  // happens AppTick times per second
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Repeater::OnStartUp()
{
  MOOSTrace("uRepeater starting....\n");
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine    = *p;
    string sVarName = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "VAR"))
      handleDeclaration(sLine);
  }

  // happens before connection is open
	
  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: handleDeclaration

bool Repeater::handleDeclaration(string str)
{
  str = stripBlankEnds(str);
  vector<string> svector = parseString(str, ',');
  if(svector.size() != 3)
    return(false);
  
  for(int i=0; i<3; i++)
    svector[i] = stripBlankEnds(svector[i]);
  
  string rpt_varname  = svector[0];
  int    rpt_count    = atoi(svector[1].c_str());
  double rpt_interval = atof(svector[2].c_str());

  bool ok = addVariable(rpt_varname, rpt_count, rpt_interval);
  return(ok);
}


//---------------------------------------------------------
// Procedure: addVariable

bool Repeater::addVariable(string var, int amt, double time)
{

  cout << "uRepeater Reg: " << var << ":" << amt << ":" << time << endl;

  int vsize = m_rpt_variable.size();
  for(int i=0; i<vsize; i++)
    if(m_rpt_variable[i] == var)
      return(false);
  if(amt <= 0)
    return(false);
  if(time <= 0)
    return(false);
  
  m_rpt_variable.push_back(var);
  m_rpt_count.push_back(amt);
  m_rpt_interval.push_back(time);
  
  m_count_remaining.push_back(0);
  m_time_stamp.push_back(MOOSTime());
  m_dfVal.push_back(0);
  m_sVal.push_back("");
  m_type.push_back("unset");

  return(true);
}


//---------------------------------------------------------
// Procedure: findVarIndex

int Repeater::findVarIndex(string var)
{
  var = stripBlankEnds(var);
  for(unsigned int i=0; i<m_rpt_variable.size(); i++) {
    if(var == m_rpt_variable[i])
      return(i);
  }
  return(-1);
}
