/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurnSet.h                                       */
/*    DATE: Jan 2nd 2023                                         */
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

  void print() const;
  
private:
  std::vector<FixedTurn> m_turns;

  // Index within the schedule
  unsigned int m_curr_ix;

  // If true the schedule repeats when completed. The curr
  // index is reset to zero.
  bool m_repeats;

};

#endif

