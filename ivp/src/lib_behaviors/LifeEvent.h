/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LifeEvent.h                                          */
/*    DATE: March 9th, 2010                                      */
/*****************************************************************/

#ifndef BEHAVIOR_LIFE_EVENT_HEADER
#define BEHAVIOR_LIFE_EVENT_HEADER

#include <string>

class LifeEvent
{
public:
  LifeEvent() {m_timestamp=0; m_iteration=0;};
  ~LifeEvent() {};
  
  void   setTimeStamp(double val)         {m_timestamp = val;};
  void   setIteration(unsigned int val)   {m_iteration = val;};
  void   setEventType(std::string str)    {m_event_type = str;};
  void   setBehaviorName(std::string str) {m_behavior_name = str;};
  void   setBehaviorType(std::string str) {m_behavior_type = str;};
  void   setSpawnString(std::string str)  {m_spawn_string = str;};
  
  double getTimeStamp() const         {return(m_timestamp);};
  unsigned int getIteration() const   {return(m_iteration);};
  std::string getEventType() const    {return(m_event_type);};
  std::string getBehaviorName() const {return(m_behavior_name);};
  std::string getBehaviorType() const {return(m_behavior_type);};
  std::string getSpawnString() const  {return(m_spawn_string);};

protected:
  double       m_timestamp;
  unsigned int m_iteration;
  std::string  m_event_type;
  std::string  m_behavior_name;
  std::string  m_behavior_type;
  std::string  m_spawn_string;
};

#endif 
