/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FixedTurn.h                                          */
/*    DATE: Jan 1st 2023                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
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
  
  void setTurnSpd(double v)     {m_turn_spd=v;}
  void setTurnModHdg(double v)  {m_turn_mod_hdg=v;}
  void setTurnFixHdg(double v)  {m_turn_fix_hdg=v;}
  void setTurnTimeOut(double v) {m_turn_timeout=v;}
  bool setTurnKey(std::string)  {m_turn_key=s;}
  bool setTurnDir(std::string);
  void addVarDataPair(VarDataPair);

  double getTurnSpd() const     {return(m_turn_spd);}
  double getTurnModHdg() const  {return(m_turn_mod_hdg);}
  double getTurnFixHdg() const  {return(m_turn_fix_hdg);}
  double getTurnTimeOut() const {return(m_turn_timeout);}
  std::string getTurnKey() const {return(m_turn_key);}
  std::string getTurnDir() const {return(m_turn_dir);}

  std::vector<VarDataPair> getFlags() const {return(m_flags);}
  
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
