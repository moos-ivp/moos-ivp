/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurnSet.cpp                                     */
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

#include <iostream>
#include "FixedTurnSet.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

FixedTurnSet::FixedTurnSet()
{
  m_curr_ix = 0;
  m_repeats = false;
}

//-----------------------------------------------------------
// Procedure: addFixedTurn()

bool FixedTurnSet::addFixedTurn(FixedTurn turn)
{
  string key = turn.getTurnKey();

  if(key != "") {
    for(unsigned int i=0; i<m_turns.size(); i++) {
      if(key == m_turns[i].getTurnKey())
	return(false);
    }
  }

  m_turns.push_back(turn);

  return(true);
}

//-----------------------------------------------------------
// Procedure: setRepeats()

bool FixedTurnSet::setRepeats(string str)
{
  return(setBooleanOnString(m_repeats, str));
}


//-----------------------------------------------------------
// Procedure: increment()

void FixedTurnSet::increment()
{
  m_curr_ix++;

  if(m_repeats && (m_curr_ix >= m_turns.size()))
    m_curr_ix = 0;
}


//-----------------------------------------------------------
// Procedure: setTurnParams()

bool FixedTurnSet::setTurnParams(string str)
{
  string lstr = tolower(str);
  // ==============================================
  // Part 1A: If just clearing, then clear and done 
  // ==============================================
  if((lstr == "clear") || (lstr == "clearall")) {
    clear();
    return(true);
  }

  // ==============================================
  // Part 1B: If clear to start, then clear and cont
  // ==============================================
  if(strBegins(lstr, "clear,") ||
     strBegins(lstr, "clearall,")) {
    clear();
    biteStringX(str, ',');
  }
  
  FixedTurn turn;
  // ==============================================
  // Part 2: If not valid specs, return false
  // ==============================================
  bool ok = turn.setTurnParams(str);
  if(!ok)
    return(false);

  // ==============================================
  // Part 3: If specs has no key, this is new turn
  // ==============================================
  if(turn.getTurnKey() == "") {
    m_turns.push_back(turn);
    return(true);
  }

  // ==============================================
  // Part 4: If specs has key, apply to proper turn
  // ==============================================
  for(unsigned int i=0; i<m_turns.size(); i++) {
    bool ok = m_turns[i].setTurnParams(str);
    if(ok)
      return(true);
  }

  // ==============================================
  // Part 5: If specs key unique, must be new turn
  // ==============================================
  m_turns.push_back(turn);
  return(true);
}

//-----------------------------------------------------------
// Procedure: getFixedTurn()
//      Note: A Null FixedTurn has default values that all
//            essentially indicate no preference. 

FixedTurn FixedTurnSet::getFixedTurn() const
{
  FixedTurn null_fixed_turn;
  
  if(m_curr_ix >= m_turns.size())
    return(null_fixed_turn);

  return(m_turns[m_curr_ix]);
}

//-----------------------------------------------------------
// Procedure: completed()

bool FixedTurnSet::completed() const
{
  if(m_repeats)
    return(false);

  if(m_curr_ix >= m_turns.size())
    return(true);

  return(false);  
}


//-----------------------------------------------------------
// Procedure: print()

void FixedTurnSet::print() const
{
  for(unsigned int i=0; i<m_turns.size(); i++) {
    cout << "Turn [" << i << "]:" << endl;
    m_turns[i].print();
  }
}

