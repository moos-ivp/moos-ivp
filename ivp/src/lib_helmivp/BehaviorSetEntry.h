/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSetEntry.h                                   */
/*    DATE: Feb 5th 2010 (Broken out from BehaviorSet)           */
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

  ~BehaviorSetEntry() {};
  
  void   setBehavior(IvPBehavior *b)    {m_behavior=b;};
  void   setState(const std::string& s) {m_state=s;};
  void   setStateTimeEntered(double v)  {m_state_time_entered=v;};
  void   setStateTimeElapsed(double v)  {m_state_time_elapsed=v;};

  void   deleteBehavior() {
    if(m_behavior) 
      delete(m_behavior);
    m_behavior = 0;
  };

  IvPBehavior* getBehavior()         {return(m_behavior);};
  std::string  getState()            {return(m_state);};
  double       getStateTimeEntered() {return(m_state_time_entered);};
  double       getStateTimeElapsed() {return(m_state_time_elapsed);};

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

