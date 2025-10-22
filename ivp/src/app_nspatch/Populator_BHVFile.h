/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_BHVFile.h                                 */
/*    DATE: July 4th, 2025                                       */
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
 
#ifndef POPULATOR_BHV_FILE_HEADER
#define POPULATOR_BHV_FILE_HEADER

#include <string>
#include "BHVFile.h"

class Populator_BHVFile {
public:
  Populator_BHVFile(bool verbose=false);
  virtual ~Populator_BHVFile() {};

  void setVerbose(bool v=true)  {m_verbose=v;}

  bool setFileBHV(std::string filename);
  bool addFileBHV(std::string filename);
  
  bool populate();

  BHVFile getBHVFile() {return(m_bhv_file);};

protected:
  bool populate(std::string filename);


  
 protected:
  BHVFile  m_bhv_file;

protected: // config vars
  
  bool m_verbose;

  std::vector<std::string> m_files_bhv;
};

#endif

