/************************************************************/
/*    NAME: Michael Benjamin, H.Schmidt, J.Leonard          */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: TS_MOOSApp.cpp                                  */
/*    DATE: May 21st 2009                                   */
/************************************************************/

#include <iterator>
#include "TS_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TS_MOOSApp::TS_MOOSApp()
{
  m_elapsed_time = 0;
  m_start_time   = 0;
}

//---------------------------------------------------------
// Destructor

TS_MOOSApp::~TS_MOOSApp()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();

//    if(key == "FOOBAR")
//      handleNewFOOBAR(sval);

  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TS_MOOSApp::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
	
  RegisterVariables();  

  return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool TS_MOOSApp::Iterate()
{
  // happens AppTick times per second
	
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TS_MOOSApp::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pTimerScript starting....\n");

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string line  = stripBlankEnds(*p);
      string param = tolower(stripBlankEnds(biteString(line, '=')));
      string value = stripBlankEnds(line);

      cout << "param: [" << param << "]" << endl;
      cout << "  value: [" << value << "]" << endl;

      // EVENT = var=FOOBAR, val=true, time=45.0
      
      if(param == "event")
	addNewEvent(value);
      
    }
  }

  printScript();
  RegisterVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: RegisterVariables

void TS_MOOSApp::RegisterVariables()
{
  m_Comms.Register("FOO_BAR", 0);
}


//------------------------------------------------------------
// Procedure: addNewEvent()
// EVENT = var=FOOBAR, val=true, time=45.0

bool TS_MOOSApp::addNewEvent(string event_str)
{
  string new_var;
  string new_val;
  double new_time_of_event = -1;

  vector<string> svector = parseStringQ(event_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(stripBlankEnds(biteString(svector[i], '=')));
    string value = stripBlankEnds(svector[i]);
    if(param == "var")
      new_var = value;
    else if(param == "val")
      new_val = value;
    else if((param == "time") && isNumber(value)) {
      double dval = atof(value.c_str());
      if(dval > 0)
	new_time_of_event = dval;
    }
  }
  
  if((new_var=="") || (new_val=="") || (new_time_of_event <=0))
    return(false);

  VarDataPair new_pair(new_var, new_val, "auto");
  m_pairs.push_back(new_pair);
  m_post_time.push_back(new_time_of_event);
  m_posted.push_back(false);

  return(true);
}


//------------------------------------------------------------
// Procedure: printScript
// 

void TS_MOOSApp::printScript()
{
  unsigned int i, vsize = m_pairs.size();
  
  cout << "The Script: ========================================" << endl;
  cout << "Total Elements: " << vsize << endl;
  for(i=0; i<vsize; i++) {
    string vdpair_str = m_pairs[i].getPrintable();
    double etime   = m_post_time[i];
    bool   posted  = m_posted[i];

    cout << "[" << i << "] " << vdpair_str << ", TIME:" << etime;
    cout << ", POSTED=" << boolToString(posted) << endl;
  }
  cout << "====================================================" << endl;
}
    


