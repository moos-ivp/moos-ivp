/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurn.cpp                                        */
/*    DATE: Jan 1st 2023                                         */
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
#include "FixedTurn.h"
#include "MBUtils.h"
#include "VarDataPairUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor()

FixedTurn::FixedTurn()
{
  // Value of -1 indicates this turn will use the default value
  // used by the user of this class, e.g. BHV_FixedTurn
  
  m_turn_spd     = -1;
  m_turn_mod_hdg = -1;
  m_turn_fix_hdg = -1;
  m_turn_timeout = -1;

  m_turn_dir = "auto";
}

//-----------------------------------------------------------
// Procedure: setTurnDir()

bool FixedTurn::setTurnDir(string str)
{
  return(setPortStarOnString(m_turn_dir, str));
}

//-----------------------------------------------------------
// Procedure: addVarDataPair()

bool FixedTurn::addVarDataPair(VarDataPair pair)
{
  if(!pair.valid())
    return(false);

  m_flags.push_back(pair);
  return(true);
}

//-----------------------------------------------------------
// Procedure: setTurnParams()

bool FixedTurn::setTurnParams(string str)
{
  string turn_spd, turn_mod_hdg, turn_fix_hdg;
  string turn_key, turn_dir, turn_timeout;

  vector<string> flag_strs;

  // ================================================
  // Part 1: gather settings, reject if unknown param
  // ================================================  
  vector<string> svector = parseStringQ(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "spd")
      turn_spd = value;
    else if(param == "mhdg")
      turn_mod_hdg = value;
    else if((param == "fhdg") || (param == "fix"))
      turn_fix_hdg = value;
    else if((param == "tdir") || (param == "turn"))
      turn_dir = tolower(value);
    else if(param == "key")
      turn_key = value;
    else if(param == "timeout")
      turn_timeout = value;
    else if(param == "flag")
      flag_strs.push_back(value);
    else
      return(false);
  }

  // ================================================
  // Part 2: Check the Key
  // ================================================  
  if((turn_key != "") && (m_turn_key != "") && 
     (turn_key != m_turn_key))
    return(false);

  // ================================================
  // Part 3: Check for suitable values
  // ================================================  
  if((turn_spd != "") && !isNumber(turn_spd))
    return(false);
  if((turn_mod_hdg != "") && !isNumber(turn_mod_hdg))
    return(false);
  if((turn_fix_hdg != "") && !isNumber(turn_fix_hdg))
    return(false);
  if((turn_timeout != "") && !isNumber(turn_timeout))
    return(false);
  if((turn_dir != "") && (turn_dir != "auto") &&
     !isValidTurn(turn_dir))
    return(false);

  vector<VarDataPair> flags;
  for(unsigned int i=0; i<flag_strs.size(); i++) {
    bool ok = addVarDataPairOnString(flags, flag_strs[i]);
    if(!ok)
      return(false);
  }

  // ================================================
  // Part 4: All parts are valid, so apply the vals
  // ================================================  
  if(turn_spd != "") 
    setTurnSpd(atof(turn_spd.c_str()));
  if(turn_mod_hdg != "")
    setTurnModHdg(atof(turn_mod_hdg.c_str()));
  if(turn_fix_hdg != "")
    setTurnFixHdg(atof(turn_fix_hdg.c_str()));
  if(turn_timeout != "")
    setTurnTimeOut(atof(turn_timeout.c_str()));
  if(turn_dir != "")
    setTurnDir(turn_dir);
  if(turn_key != "")
    setTurnKey(turn_key);

  for(unsigned int i=0; i<flags.size(); i++)
    m_flags.push_back(flags[i]);

  return(true);
}

//-----------------------------------------------------------
// Procedure: print()

void FixedTurn::print() const
{
  cout << "turn_spd:     " << doubleToStringX(m_turn_spd) << endl;
  cout << "turn_mod_hdg: " << doubleToStringX(m_turn_mod_hdg) << endl;
  cout << "turn_fix_hdg: " << doubleToStringX(m_turn_fix_hdg) << endl;
  cout << "turn_timeout: " << doubleToStringX(m_turn_timeout) << endl;
  cout << "turn_key:     " << m_turn_key << endl;
  cout << "turn_dir:     " << m_turn_dir << endl;
  for(unsigned int i=0; i<m_flags.size(); i++)
    cout << m_flags[i].getPrintable() << endl;
}
  

