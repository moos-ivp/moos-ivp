/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_MOOSFile.h                                 */
/*    DATE: May 7th, 2025                                        */
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
 
#ifndef POPULATOR_MOOS_FILE_HEADER
#define POPULATOR_MOOS_FILE_HEADER

#include <string>
#include "MOOSFile.h"

class Populator_MOOSFile {
public:
  Populator_MOOSFile(bool verbose=false);
  virtual ~Populator_MOOSFile() {};

  void setVerbose(bool v=true)  {m_verbose=v;}

  bool setFileMOOS(std::string filename);
  bool addFileMOOS(std::string filename);
  
  bool populate();

  MOOSFile getMOOSFile() {return(m_moos_file);};

protected:
  bool populate(std::string filename);
  
 protected:
  MOOSFile    m_moos_file;

protected: // config vars
  
  bool m_verbose;

  std::vector<std::string> m_files_moos;
};

#endif

