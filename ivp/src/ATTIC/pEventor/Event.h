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
  EventEntry(std::string id);
  virtual ~Eventor() {};
  
 protected:

protected: // Configuration states
  std::string       m_id;
  std::string       m_varname;
  std::string       m_vartype;
  std::string       m_sval;
  double            m_dval;
  std::string       m_repeats_want;
  std::string       m_repeats_done;
  double            m_time_to_event;
  double            m_init_time;
  bool              m_time_initialized;

  std::vector<std::string> m_trigger_varname;
  std::vector<std::string> m_trigger_vartype;
  std::vector<std::string> m_trigger_sval;
  std::vector<std::string> m_trigger_dval;
  
};

#endif
