/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurn.cpp                                        */
/*    DATE: Jan 1st 2023                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "FixedTurn.h"
#include "MBUtils.h"

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
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "spd")
      turn_spd = value;
    else if(param == "mhdg")
      turn_mod_hdg = value;
    else if(param == "fhdg")
      turn_fix_hdg = value;
    else if(param == "tdir")
      turn_dir = value;
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
  if((turn_key != "") && (turn_key != m_turn_key))
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
  if((turn_dir != "") && !isValidTurn(turn_dir))
    return(false);

  vector<VarDataPair> flags;
  for(unsigned int i=0; i<flag_strs.size(); i++) {
    VarDataPair pair = stringToVarDataPair(flag_strs[i]);
    if(!pair.valid())
      return(false);
    else
      flags.push_back(pair);
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

  for(unsigned int i=0; i<flags.size(); i++)
    m_flags.push_back(flags[i]);

  return(true);
}

