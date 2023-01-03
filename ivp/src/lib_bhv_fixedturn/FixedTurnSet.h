/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurnSet.h                                       */
/*    DATE: Jan 2nd 2023                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef FIXED_TURN_SET_HEADER
#define FIXED_TURN_SET_HEADER

#include <string>
#include <vector>
#include "FixedTurn.h"

class FixedTurnSet {
public:
  FixedTurnSet();
  ~FixedTurnSet() {}

  bool addFixedTurn(FixedTurn);
  
  bool setTurnParams(std::string);
  bool setRepeats(std::string);

  void increment();
  void clear() {m_turns.clear(); m_curr_ix=0;}

  FixedTurn getFixedTurn() const;

  unsigned int size() const {return(m_turns.size());}

  bool completed() const;

  
private:
  std::vector<FixedTurn> m_turns;

  // Index within the schedule
  unsigned int m_curr_ix;

  // If true the schedule repeats when completed. The curr
  // index is reset to zero.
  bool m_repeats;

};

#endif
