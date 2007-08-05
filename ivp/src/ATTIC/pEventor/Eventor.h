/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Eventor.h                                            */
/*    DATE: Aug 20th 2006                                        */
/*****************************************************************/

#ifndef P_EVENTOR_HEADER
#define P_EVENTOR_HEADER

#include <string>
#include <vector>
#include "MOOSLib.h"
#include "EventEntry.h"

class Eventor : public CMOOSApp
{
 public:
  Eventor();
  virtual ~Eventor() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected:
  void printStaticSchedule();
  void printDynamicSchedule();

  void initializeStartTime();
  void conditionalExecute();

  int  size() {return(m_varname.size());};

  bool handleNewEvent(std::string);
  bool handleNewTrigger(std::string);
  int  getEntryIndex(std::string);

protected: // Configuration states
  std::vector<std::string>   m_varname;
  std::vector<bool>          m_isdouble;
  std::vector<double>        m_dvalue;
  std::vector<std::string>   m_svalue;
  std::vector<int>           m_repeats_want;
  std::vector<double>        m_time_to_event;
  std::vector<double>        m_init_time;

protected: // Runtime states

  std::vector<EventEntry>    m_event_entries;

  std::vector<int>           m_repeats_done;

  int                        m_iteration;
  bool                       m_start_time_initialized;
  bool                       m_verbose;
};

#endif
