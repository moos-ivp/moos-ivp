/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LifeEvent.h                                          */
/*    DATE: March 9th, 2010                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef BEHAVIOR_LIFE_EVENT_HEADER
#define BEHAVIOR_LIFE_EVENT_HEADER

#include <string>

class LifeEvent
{
public:
  LifeEvent();
  ~LifeEvent() {}
  
  void   setTimeStamp(double val)         {m_timestamp = val;}
  void   setIteration(unsigned int val)   {m_iteration = val;}
  void   setEventType(std::string str)    {m_event_type = str;}
  void   setBehaviorName(std::string str) {m_behavior_name = str;}
  void   setBehaviorType(std::string str) {m_behavior_type = str;}
  void   setSpawnString(std::string str)  {m_spawn_string = str;}
  void   setPostingIndex(int val)         {m_posting_index = val;}
  
  double getTimeStamp() const         {return(m_timestamp);}
  unsigned int getIteration() const   {return(m_iteration);}
  std::string getEventType() const    {return(m_event_type);}
  std::string getBehaviorName() const {return(m_behavior_name);}
  std::string getBehaviorType() const {return(m_behavior_type);}
  std::string getSpawnString() const  {return(m_spawn_string);}
  int getPostingIndex() const         {return(m_posting_index);}

  bool operator==(const LifeEvent&);
  
protected:
  double       m_timestamp;
  unsigned int m_iteration;
  std::string  m_event_type;
  std::string  m_behavior_name;
  std::string  m_behavior_type;
  std::string  m_spawn_string;
  int          m_posting_index;
};

#endif 










