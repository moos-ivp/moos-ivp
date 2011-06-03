/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LifeEvent.h                                          */
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

