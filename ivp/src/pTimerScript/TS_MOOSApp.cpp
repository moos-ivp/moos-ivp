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
  m_skip_time    = 0;
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
  double curr_time = MOOSTime();
  if(m_start_time == 0)
    m_start_time = curr_time;

  m_elapsed_time = (curr_time - m_start_time) + m_skip_time;
  
  checkForPostings();

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
  m_ptime.push_back(new_time_of_event);
  m_poked.push_back(false);

  sortEvents();
  return(true);
}

//------------------------------------------------------------
// Procedure: sortEvents
// 

void TS_MOOSApp::sortEvents()
{
  unsigned int count = 0;
  unsigned int i, vsize = m_pairs.size();
  vector<bool> sorted(vsize,false);
  
  vector<VarDataPair> new_pairs;
  vector<double>      new_ptime;
  vector<bool>        new_poked;

  while(count < vsize) {
    unsigned int oldest_index = 0;
    double       oldest_ptime = -1;
    for(i=0; i<vsize; i++) {
      if((m_ptime[i] > oldest_ptime) && !sorted[i]) {
	oldest_ptime = m_ptime[i];
	oldest_index = i;
      }
    }
    sorted[oldest_index] = true;
    new_pairs.push_back(m_pairs[oldest_index]);
    new_ptime.push_back(m_ptime[oldest_index]);
    new_poked.push_back(m_poked[oldest_index]);
    count++;
  }

  m_pairs = new_pairs;
  m_ptime = new_ptime;
  m_poked = new_poked;
  std::reverse(m_pairs.begin(), m_pairs.end());
  std::reverse(m_ptime.begin(), m_ptime.end());
  std::reverse(m_poked.begin(), m_poked.end());
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
    double ptime  = m_ptime[i];
    bool   poked  = m_poked[i];

    cout << "[" << i << "] " << vdpair_str << ", TIME:" << ptime;
    cout << ", POSTED=" << boolToString(poked) << endl;
  }
  cout << "====================================================" << endl;
}
    
//------------------------------------------------------------
// Procedure: checkForPostings()
//   Purpose: Go through the list of events and possibly post the
//            event if the elapsed time is greater or equal to the
//            post time, and if not posted previously.

void TS_MOOSApp::checkForPostings()
{
  unsigned int i, vsize = m_pairs.size();
  for(i=0; i<vsize; i++) {
    if((m_elapsed_time > m_ptime[i]) && !m_poked[i]) {
      string variable = m_pairs[i].get_var();
      if(m_pairs[i].is_string()) 
	m_Comms.Notify(variable, m_pairs[i].get_sdata());
      else
	m_Comms.Notify(variable, m_pairs[i].get_ddata());
      m_poked[i] = true;
    }
    else // Since ordered events, return on first failure.
      return;
  }
}


