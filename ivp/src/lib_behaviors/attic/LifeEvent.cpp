/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LifeEvent.cpp                                        */
/*    DATE: Sep 1st, 2019                                        */
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

#include "LifeEvent.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

LifeEvent::LifeEvent()
{
  m_timestamp = 0;
  m_iteration = 0;
  m_posting_index = -1;
}

//-----------------------------------------------------------
// Procedure: Overload operator ==

bool LifeEvent::operator==(const LifeEvent& event)
{
  if(m_timestamp != event.getTimeStamp())
    return(false);
  if(m_iteration != event.getIteration())
    return(false);
  if(m_event_type != event.getEventType())
    return(false);
  if(m_behavior_name != event.getBehaviorName())
    return(false);
  if(m_behavior_type != event.getBehaviorType())
    return(false);
  if(m_spawn_string != event.getSpawnString())
    return(false);
  if(m_posting_index != event.getPostingIndex())
    return(false);

  return(true);
}



