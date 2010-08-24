/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LifeEventHistory.h                                   */
/*    DATE: March 9th, 2010                                      */
/*****************************************************************/

#ifndef LIFE_EVENT_HISTORY_HEADER
#define LIFE_EVENT_HISTORY_HEADER

#include <string>
#include <vector>
#include "LifeEvent.h"

class LifeEventHistory
{
public:
  LifeEventHistory();
  ~LifeEventHistory() {};
  
  void addLifeEvent(const LifeEvent& event);
  void addLifeEvent(const std::string& event);

  bool isStale() {return(m_stale_report);};

  void setBannerActive(bool v) {m_banner_active=v;};
  void setColorActive(bool v)  {m_color_active=v;};
  void setSeedActive(bool v)   {m_seed_active=v;};

  std::vector<std::string> getReport(std::string mode="normal");

protected:

  std::vector<LifeEvent>   m_life_events;
  std::vector<std::string> m_history_report;

  bool m_stale_report;

  bool m_banner_active;
  bool m_color_active;
  bool m_seed_active;

  unsigned int m_max_len_time;
  unsigned int m_max_len_iter;
  unsigned int m_max_len_bname;
  unsigned int m_max_len_btype;
  unsigned int m_max_len_event;
  unsigned int m_max_len_seed;
};

#endif 
