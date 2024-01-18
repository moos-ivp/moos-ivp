/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BehaviorSetEntry.h                                   */
/*    DATE: Feb 5th 2010 (Broken out from BehaviorSet)           */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef BEHAVIORSET_ENTRY_HEADER
#define BEHAVIORSET_ENTRY_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"

class BehaviorSetEntry
{
public:
  BehaviorSetEntry(IvPBehavior *b=0) {
    m_behavior = b;
    m_state = "";
    m_state_time_entered = 0;
    m_state_time_elapsed = -1;
  }

  ~BehaviorSetEntry() {}
  
  void   setBehavior(IvPBehavior *b)    {m_behavior=b;}
  void   setState(const std::string& s) {m_state=s;}
  void   setStateTimeEntered(double v)  {m_state_time_entered=v;}
  void   setStateTimeElapsed(double v)  {m_state_time_elapsed=v;}
  void   setPlatModel(const PlatModel& pmodel)
  {if(m_behavior) m_behavior->setPlatModel(pmodel);}

  void   deleteBehavior() {
    if(m_behavior) 
      delete(m_behavior);
    m_behavior = 0;
  }

  IvPBehavior* getBehavior()         {return(m_behavior);}
  std::string  getState()            {return(m_state);}
  double       getStateTimeEntered() {return(m_state_time_entered);}
  double       getStateTimeElapsed() {return(m_state_time_elapsed);}

  std::string  getBehaviorName()  {
    if(m_behavior)
      return(m_behavior->getDescriptor());
    return("unknown");
  }

  std::string  getBehaviorType()  {
    if(m_behavior)
      return(m_behavior->getBehaviorType());
    return("unknown");
  }

protected:
  IvPBehavior*   m_behavior;
  std::string    m_state;
  double         m_state_time_entered;
  double         m_state_time_elapsed;
};

#endif 

