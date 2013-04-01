/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: TestClassifyQueue.cpp                           */
/*    DATE: March 23rd, 2013                                */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "TestClassifyQueue.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TestClassifyQueue::TestClassifyQueue()
{
  m_queue_time    = 10; // seconds
  m_last_pop_time = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TestClassifyQueue::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

    bool handled  = false;
    
#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "CLASSIFY_ENTRY") 
      handled = handleMailClassifyEntry(sval);
    
    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TestClassifyQueue::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool TestClassifyQueue::Iterate()
{
  AppCastingMOOSApp::Iterate();

  double elapsed = MOOSTime() - m_last_pop_time;
  if(elapsed > m_queue_time) {
    ClassifyEntry entry = m_classify_queue.pop();
    m_last_pop_time = MOOSTime();
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool TestClassifyQueue::OnStartUp()
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
    if((param == "queue_time") && isNumber(value)) {
      double qtime = atof(value.c_str());
      if(qtime >= 0) {
	m_queue_time = qtime;
	handled = true;
      }
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables
// Note: CLASSIFY_ENTRY = "label=239, priority=45"

void TestClassifyQueue::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("CLASSIFY_ENTRY", 0);
}

//---------------------------------------------------------
// Procedure: handleMailClassifyEntry
// Note: CLASSIFY_ENTRY = "label=239, priority=45"

bool TestClassifyQueue::handleMailClassifyEntry(string str)
{
  string label, priority;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize  = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "label")
      label = value;
    else if(param == "priority")
      priority = value;
  }

  if((label == "") || !isNumber(priority)) 
    return(false);
 
  ClassifyEntry entry;
  XYHazard hazard;
  hazard.setLabel(label);
  entry.setHazard(hazard);

  entry.setPriority(atof(priority.c_str()));
  entry.setTimeReq(MOOSTime());
  m_classify_queue.push(entry);
  return(true);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool TestClassifyQueue::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "QueueSize: " << m_classify_queue.size()       << endl;
  m_msgs << "============================================" << endl;;

  ACTable actab(3,2);
  actab << "Label | Priority | Elapsed Time";
  actab.addHeaderLines();
  
  vector<ClassifyEntry> ce_vector = m_classify_queue.getCopyAllEntries();
  unsigned int i, vsize = ce_vector.size();
  for(i=0; i<vsize; i++) {
    string label    = ce_vector[i].getHazard().getLabel();
    double priority = ce_vector[i].getPriority();
    double elapsed  = MOOSTime() - ce_vector[i].getTimeReq();
    
    actab << label;
    actab << doubleToStringX(priority,9);
    actab << doubleToString(elapsed,2);
  }

  m_msgs << actab.getFormattedString();
  return(true);
}




