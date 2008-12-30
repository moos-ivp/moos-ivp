/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ModeSet.h                                            */
/*    DATE: Dec 26th, 2008                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef MODE_SET_HEADER
#define MODE_SET_HEADER

#include <vector>
#include <string>
#include "IvPDomain.h"
#include "ModeEntry.h"

class ModeSet {
public:
  ModeSet() {};
  ~ModeSet() {};

  bool addEntry(const std::string&);

  unsigned int size() {return(m_entries.size());};
  
  //std::vector<std::string> getMOOSVars()  {return(m_moos_vars);};
  //std::string getModeVar(unsigned int ix) {return(m_mode_vars[ix]);}; 
  //std::string getModeVal(unsigned int ix) {return(m_mode_vals[ix]);}; 

  std::vector<std::string> getConditionVars();
  void print();


protected:
  std::vector<ModeEntry>  m_entries;
};

#endif
