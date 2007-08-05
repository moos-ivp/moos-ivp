/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: EventEntry.h                                         */
/*    DATE: Aug 22nd 2006                                        */
/*****************************************************************/

#ifndef EVENT_ENTRY_HEADER
#define EVENT_ENTRY_HEADER

#include <string>
#include <vector>
#include "MOOSLib.h"

class EventEntry
{
 public:
  EventEntry(std::string id, std::string varname, std::string vartype,
	     std::string sval, double dval, int repeats, double delay);
  virtual ~EventEntry() {};
  
  std::string getID()          {return(m_id);};
  std::string getVarName()     {return(m_varname);};
  std::string getVarType()     {return(m_vartype);};
  std::string getSVal()        {return(m_sval);};
  double      getDVal()        {return(m_dval);};
  int         getWRepeats( )   {return(m_repeats_want);};
  int         getDRepeats( )   {return(m_repeats_done);};
  double      getDelay()       {return(m_delay);};
  double      getInitTime()    {return(m_init_time);};
  bool        getInitialized() {return(m_initialized);};

  void        addTrigger(std::string, std::string);
  void        setInitTime(double);
  void        applyMessage(std::string, double, double);
  void        applyMessage(std::string, std::string, double);
  void        reset(double);
  bool        eventReady(double);
    
  std::string print(bool=true);

  std::vector<std::string> getTriggerVariables();

protected: // Configuration states
  std::string    m_id;
  std::string    m_varname;
  std::string    m_vartype;
  std::string    m_sval;
  double         m_dval;
  int            m_repeats_want;
  int            m_repeats_done;
  double         m_delay;
  double         m_init_time;
  bool           m_initialized;
  bool           m_triggered;

  std::vector<std::string> m_trigger_varname;
  std::vector<std::string> m_trigger_vartype;
  std::vector<std::string> m_trigger_sval;
  std::vector<double>      m_trigger_dval;
  
};

#endif
