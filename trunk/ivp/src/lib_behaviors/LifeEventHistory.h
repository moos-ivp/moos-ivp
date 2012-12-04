/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LifeEventHistory.h                                   */
/*    DATE: March 9th, 2010                                      */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
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


