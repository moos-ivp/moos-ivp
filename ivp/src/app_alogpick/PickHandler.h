/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PickHandler.h                                        */
/*    DATE: August 13th, 2023                                    */
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

#ifndef PICK_HANDLER_HEADER
#define PICK_HANDLER_HEADER

#include <string>
#include <map>

class PickHandler
{
 public:
  PickHandler();
  ~PickHandler() {}

  bool handle();
  
  bool setLogFile(std::string);
  bool addField(std::string);
  void setVerbose() {m_verbose=true;}
  void setFormatAligned(bool v) {m_format_aligned=v;}
  
 protected: // Config vars

  bool   m_verbose;
  bool   m_format_aligned;

  std::vector<std::string> m_fields;

  std::vector<std::vector<std::string> > m_values;
  
 protected: // State vars

  FILE *m_file_in;
};

#endif

