/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: Populator_TaskDiary.h                                */
/*    DATE: Nov 23rd, 2023                                       */
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

#ifndef POPULATOR_OPFIELD_HEADER
#define POPULATOR_OPFIELD_HEADER

#include <string>
#include <vector>
#include "OpField.h"

class Populator_OpField {
public:
  Populator_OpField() {m_verbose=false;}
  ~Populator_OpField() {}

  bool    addFileOPF(std::string filename);
  bool    populate();
  void    setVerbose() {m_verbose = true;}
  OpField getOpField() {return(m_opfield);}

protected:
  bool    populateFromFile(std::string);
  
protected:
  bool    m_verbose;
  OpField m_opfield;

  std::vector<std::string> m_opf_files;
};
#endif 
