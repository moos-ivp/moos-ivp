/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurnSet.cpp                                     */
/*    DATE: Jan 2nd 2023                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "FixedTurnSet.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

FixedTurnSet::FixedTurnSet()
{
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
// Procedure: setTurnParams()

bool FixedTurnSet::setTurnParams(string str)
{
  FixedTurn turn;

  // ==============================================
  // Part 1: If not valid specs, return false
  // ==============================================
  bool ok = turn.setTurnParams(str);
  if(!ok)
    return(false);

  // ==============================================
  // Part 2: If specs has no key, this is new turn
  // ==============================================
  if(turn.getTurnKey() == "") {
    m_turns.push_back(turn);
    return(true);
  }

  // ==============================================
  // Part 3: If specs has key, apply to proper turn
  // ==============================================
  for(unsigned int i=0; i<m_turns.size(); i++) {
    bool ok = m_turns[i].setTurnParams(str);
    if(ok)
      return(true);
  }

  // ==============================================
  // Part 4: If specs key unique, must be new turn
  // ==============================================
  m_turns.push_back(turn);
  return(true);
}

