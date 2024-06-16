/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurn.h                                          */
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

#ifndef FIXED_TURN_HEADER
#define FIXED_TURN_HEADER

#include <string>
#include <vector>
#include "VarDataPair.h"

class FixedTurn {
public:
  FixedTurn();
  ~FixedTurn() {}

  bool setTurnParams(std::string);
  
  void setTurnSpd(double v)      {m_turn_spd=v;}
  void setTurnModHdg(double v)   {m_turn_mod_hdg=v;}
  void setTurnFixHdg(double v)   {m_turn_fix_hdg=v;}
  void setTurnTimeOut(double v)  {m_turn_timeout=v;}
  void setTurnKey(std::string s) {m_turn_key=s;}
  bool setTurnDir(std::string);
  bool addVarDataPair(VarDataPair);

  double getTurnSpd() const     {return(m_turn_spd);}
  double getTurnModHdg() const  {return(m_turn_mod_hdg);}
  double getTurnFixHdg() const  {return(m_turn_fix_hdg);}
  double getTurnTimeOut() const {return(m_turn_timeout);}
  std::string getTurnKey() const {return(m_turn_key);}
  std::string getTurnDir() const {return(m_turn_dir);}

  std::vector<VarDataPair> getFlags() const {return(m_flags);}

  void print() const;
  
private:
  
  double m_turn_spd;
  double m_turn_mod_hdg;
  double m_turn_fix_hdg;
  double m_turn_timeout;
  
  std::string m_turn_key;
  std::string m_turn_dir;

  std::vector<VarDataPair> m_flags;
};

#endif

