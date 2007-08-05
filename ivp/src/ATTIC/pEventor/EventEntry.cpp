/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: EventEntry.cpp                                       */
/*    DATE: Aug 22nd 2006                                        */
/*****************************************************************/

#include <iostream>
#include <iterator>
#include "EventEntry.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

EventEntry::EventEntry(string g_id, string g_varname, 
		       string g_vartype, string g_sval, double g_dval, 
		       int g_repeats, double g_delay)
{
  // A unique identifier for the event. Cannot use varname since 
  // since there could be distict events w/ the same variable
  m_id            = g_id;

  // The Variable name to be posted upon the event
  m_varname       = g_varname;

  // The variable type - either string or double
  m_vartype       = g_vartype;

  // The string value to be posted if it is a string type 
  m_sval          = g_sval;
  
  // The double value to be posted if it is a double type 
  m_dval          = g_dval;
  
  // The number of times the event is to be repeated
  m_repeats_want  = g_repeats;
  
  // The number of times the event is to be repeated
  
  // The number of times the event has been repeated so far
  m_repeats_done  = 0;
  
  // Time in seconds from trigger (if any) to event execution
  m_delay         = g_delay;
  
  // Trigger start time for the event
  m_init_time     = 0;

  // Flag indicating that the init_time was set. For events with
  // no triggers, init_time is system time at the outsetd and 
  // must be set at the outset. If this is false, m_init_time 
  // is not trusted
  m_initialized   = false;

  // Unless there are trigger variables, this event-entry is 
  // always in triggered mode. This mode is true iff any or all
  // trigger conditions are met.
  m_triggered     = true;
}


//-----------------------------------------------------------------
// Procedure: addTrigger

void EventEntry::addTrigger(string var, string val)
{
  string varname = var;
  string vartype = "string";
  string sval    = "";
  double dval    = 0;

  if(isNumber(val)) {
    vartype = "double";
    dval = atof(val.c_str());
  }
    
  if(isQuoted(val))
    val = stripQuotes(val);
  sval = val;
  
  m_trigger_varname.push_back(varname);
  m_trigger_vartype.push_back(vartype);
  m_trigger_sval.push_back(sval);
  m_trigger_dval.push_back(dval);

  // As soon as a new trigger condition is added, the event-entry
  // is assumed to be not triggered.
  m_triggered = false;
}


//-----------------------------------------------------------------
// Procedure: setInitTime
//
//   The init_time is the time when the count-down has begun to the 
//   event executon. The duration of the count-down is represented 
//   by m_delay. For event-entrie with triggers, this count-down 
//   begins when the *first* trigger condition is met. For 
//   event-entries with no trigger, this init_time is set by
//   a caller calling this function at the outset of the process 
//   containing this event-entry.

void EventEntry::setInitTime(double init_time)
{
  if(init_time < 0)
    return;
  m_init_time   = init_time;
  m_initialized = true;
}

//-----------------------------------------------------------------
// Procedure: applyMessage  <string>
//
//
//   Apply a message, a var-name,var-value pair, where the var-value
//   is of type string, to the list of trigger conditions. The sole
//   result is to either flip the m_triggered value to true or not.
//   If m_triggered is already true, no change is affected.

void EventEntry::applyMessage(string g_var, string g_val, 
			      double g_curr_time)
{
  if(m_triggered)
    return;

  g_var = stripBlankEnds(g_var);
  g_val = stripBlankEnds(g_val);
  
  int vsize = m_trigger_varname.size();
  for(int i=0; ((i<vsize)&&(!m_triggered)); i++)
    if(m_trigger_vartype[i] == "string")
      if(m_trigger_varname[i] == g_var)
	if(m_trigger_sval[i] == g_val)
	  m_triggered = true;
  
  if(m_triggered)
    m_init_time = g_curr_time;
}

//-----------------------------------------------------------------
// Procedure: applyMessage  <double>
//
//   Apply a message, a var-name,var-value pair, where the var-value
//   is of type double, to the list of trigger conditions. The sole
//   result is to either flip the m_triggered value to true or not.
//   If m_triggered is already true, no change is affected.

void EventEntry::applyMessage(string g_var, double g_val, 
			      double g_curr_time)
{
  if(m_triggered)
    return;

  g_var = stripBlankEnds(g_var);
  
  int vsize = m_trigger_varname.size();
  for(int i=0; ((i<vsize)&&(!m_triggered)); i++)
    if(m_trigger_vartype[i] == "double")
      if(m_trigger_varname[i] == g_var)
	if(m_trigger_dval[i] == g_val)
	  m_triggered = true;
  
  if(m_triggered)
    m_init_time = g_curr_time;
}


//-----------------------------------------------------------------
// Procedure: reset
//

void EventEntry::reset(double g_curr_time)
{
  m_triggered = false;
  m_init_time = g_curr_time;
}


//-----------------------------------------------------------------
// Procedure: eventReady
//

bool EventEntry::eventReady(double g_curr_time)
{
  if(!m_triggered)
    return(false);

  if((g_curr_time - m_init_time) > m_delay)
    return(true);
  else
    return(false);
}


//-----------------------------------------------------------------
// Procedure: getTriggerVariables
//

vector<string> EventEntry::getTriggerVariables()
{
  return(m_trigger_varname);
}


//-----------------------------------------------------------------
// Procedure: print()
//

string EventEntry::print(bool to_screen)
{
  string output;

  output += "id="  + m_id + ","; 
  output += "var=" + m_varname + ","; 
  if(m_vartype == "string") 
    output += "val=\"" + m_sval + "\",";
  else
    output += "val=" + doubleToString(m_dval) + ",";
  output += "repeat=" + doubleToString(m_repeats_want) + ",";
  output += "delay="  + doubleToString(m_delay);
  
  int vsize = m_trigger_varname.size();
  for(int i=0; i<vsize; i++) {
    output += ",trigger_var=" + m_trigger_varname[i];
    if(m_trigger_vartype[i] == "string")
      output += ",trigger_val=\"" + m_trigger_sval[i] + "\"";
    else
      output += ",trigger_val=" + doubleToString(m_trigger_dval[i]) + "\"";
  }
  
  if(to_screen)
    cout << output << endl;
  return(output);
}

