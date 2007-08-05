/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Aug 20th 2006                                        */
/*****************************************************************/

#include <iostream>
#include <iterator>
#include "Eventor.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

Eventor::Eventor()
{
  m_iteration = 0;
  m_verbose   = false;
  m_start_time_initialized = false;
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool Eventor::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  double curr_time = MOOSTime() - getAppStartTime();

  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    
    string key   = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    int vsize = m_event_entries.size();
    if(mtype == MOOS_DOUBLE) {
      for(int i=0; i<vsize; i++) {
	m_event_entries[i].applyMessage(key, sdata, curr_time);
      }
    }
      
    if(mtype == MOOS_STRING) {
      for(int i=0; i<vsize; i++) {
	m_event_entries[i].applyMessage(key, ddata, curr_time);
      }
    }
    

  }
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectToServer

bool Eventor::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate

bool Eventor::Iterate()
{
  m_iteration++;

  printStaticSchedule();
  printDynamicSchedule();

  if(!m_start_time_initialized)
    initializeStartTime();

  conditionalExecute();

  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnStartUp()

bool Eventor::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);
    
    if(MOOSStrCmp(sVarName, "EVENT")) {
      bool ok = handleNewEvent(sLine);
      if(!ok) {
	MOOSDebugWrite("Illegal EVENT syntax - " + sLine);
	return(false);
      }
    }

    if(MOOSStrCmp(sVarName, "TRIGGER")) {
      bool ok = handleNewTrigger(sLine);
      if(!ok) {
	MOOSDebugWrite("Illegal TRIGGER syntax - " + sLine);
	return(false);
      }
    }

    if(MOOSStrCmp(sVarName, "VERBOSE")) {
      sLine = tolower(sLine);
      if((sLine!="true")&&(sLine!="false")&&(sLine!="verbose")) {
	MOOSDebugWrite("Illegal VERBOSE syntax");
	return(false);
      }
      m_verbose = ((sLine=="true")||(sLine=="verbose"));
    }
  }
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: printDynamicSchedule()

void Eventor::printDynamicSchedule()
{
  int vsize = m_varname.size();

  cout << "Current Schedule Configuration: [";
  cout << m_iteration << "]" << endl;
  cout << "Items: " << vsize << endl;
  for(int i=0; i<vsize; i++) {
    cout << "[" << i << "] " << m_varname[i];
    if(m_isdouble[i])
      cout << "  (double) " << endl;
    else
      cout << "\" m_svalue[i] \"" << endl;
    cout << "  remaining repeats:" << 
      m_repeats_want[i] - m_repeats_done[i] << endl;
    cout << "  time_to_event: " << m_time_to_event[i] << endl;
  }
  

}

//-----------------------------------------------------------------
// Procedure: printStaticSchedule()

void Eventor::printStaticSchedule()
{
  int vsize = m_event_entries.size();

  cout << "Current Schedule Configuration: [";
  cout << m_iteration << "]" << endl;
  cout << "Items: " << vsize << endl;
  for(int i=0; i<vsize; i++) {
    cout << "[" << i << "]: ";
    m_event_entries[i].print();
  }
}

//-----------------------------------------------------------------
// Procedure: conditionalExecute()

void Eventor::conditionalExecute()
{
  int    vsize     = m_varname.size();
  double curr_time = MOOSTime();

  cout << "curr_time: " << MOOSTime();

  for(int i=0; i<vsize; i++) {
    double delta = curr_time - m_init_time[i];

    bool i_done = false;
    if(m_repeats_done[i] > m_repeats_want[i])
      i_done = true;

    cout << "examining " << m_varname[i] << " delta: " << delta << endl;
    cout << "m_init_time: " << m_init_time[i] << endl;
    if(!i_done && (delta > m_time_to_event[i])) {
      cout << "Posting event: " << m_varname[i] << endl;
      if(m_isdouble[i])
	m_Comms.Notify(m_varname[i], m_dvalue[i]);
      else
	m_Comms.Notify(m_varname[i], m_svalue[i]);
      
      m_repeats_done[i]++;
      m_init_time[i] = curr_time;
    }
  }
}


//-----------------------------------------------------------------
// Procedure: initializeStartTime

void Eventor::initializeStartTime()
{
  if(m_start_time_initialized == true)
    return;
  
  double curr_time = MOOSTime();

  int vsize = m_varname.size();
  for(int i=0; i<vsize; i++)
    m_init_time[i] = curr_time;
  
  m_start_time_initialized = true;
}


//-----------------------------------------------------------------
// Procedure: 

bool Eventor::handleNewEvent(string new_event)
{
  string id;
  string varname;
  string vartype;
  string sval;
  double dval;
  int    repeat = 0;  // Default allowed for this variable
  double delay  = 0;  // Default allowed for this variable

  bool   id_set      = false;
  bool   varname_set = false;
  bool   varval_set  = false;

  vector<string> svector = parseString(new_event, ',');

  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    vector<string> svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2)
      return(false);
    string key    = tolower(stripBlankEnds(svector2[0]));
    string keyval = svector2[1];
    
    if(key == "id") {
      id = keyval; 
      id_set = true;
    }
    if(key == "var") {
      varname = keyval;
      varname_set = true;
    }
    if(key == "val") {
      if(isNumber(keyval)) {
	vartype = "double";
	dval = atof(keyval.c_str());
      }
      else {
	if(isQuoted(keyval))
	  keyval = stripQuotes(keyval);
	sval = keyval;
      }
      varval_set = true;
    }
    if(key == "repeat") {
      if(!isNumber(keyval))
	return(false);
      repeat = atoi(keyval.c_str());
    }
    if(key == "delay") {
      if(!isNumber(keyval))
	return(false);
      delay = atof(keyval.c_str());
    }
  }

  // Check mandatory fields have been set
  if(!id_set || !varname_set || !varval_set)
    return(false);

  // Insist on having a unique ID before adding new entry.
  int entry_index = getEntryIndex(id);
  if(entry_index != -1)
    return(false);
    

  EventEntry new_entry(id, varname, vartype, sval, dval, repeat, delay);

  double curr_time = MOOSTime() - GetAppStartTime();
  new_entry.setInitTime(curr_time);

  m_event_entries.push_back(new_entry);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: 

bool Eventor::handleNewTrigger(string new_trigger)
{
  string id;
  string varname;
  string varvalue;

  bool   id_set      = false;
  bool   varname_set = false;
  bool   varval_set  = false;

  vector<string> svector = parseString(new_trigger, ',');

  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    vector<string> svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2)
      return(false);
    string key    = tolower(stripBlankEnds(svector2[0]));
    string keyval = svector2[1];
    
    if(key == "id") {
      id = keyval; 
      id_set = true;
    }
    if(key == "var") {
      varname = keyval;
      varname_set = true;
    }
    if(key == "val") {
      varvalue = keyval;
      varval_set = true;
    }
  }

  if(!id_set || !varname_set || !varval_set)
    return(false);

  int entry_index = getEntryIndex(id);
  if(entry_index == -1)
    return(false);
  
  m_event_entries[entry_index].addTrigger(varname, varvalue);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: getEntryIndex

int Eventor::getEntryIndex(std::string g_id)
{
  int vsize = m_event_entries.size();
  for(int i=0; i<vsize; i++) 
    if(g_id == m_event_entries[i].getID())
      return(i);

  return(-1);
}
