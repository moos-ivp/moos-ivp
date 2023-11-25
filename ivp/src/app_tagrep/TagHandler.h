/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TagHandler.h                                         */
/*    DATE: Sep 28th, 2020                                       */
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

#ifndef TAG_HANDLER_HEADER
#define TAG_HANDLER_HEADER

#include <string>
#include <map>

class TagHandler
{
 public:
  TagHandler();
  ~TagHandler() {}

  bool handle();
  
  bool setInputFile(std::string);
  bool setTag(std::string);
  bool setHeader(std::string);

  void setKeepTagLine()   {m_keep_tag_line=true;}
  void setFirstLineOnly() {m_first_line_only=true;}
  void setNoBlankLines()  {m_keep_blanks=false;}
  void setVerbose()       {m_verbose=true;}

  bool isInputFileSet() {return(m_input_file!="");}
  
 protected: // Config vars
  std::string m_input_file;
  std::string m_tag;
  std::string m_header;

  bool  m_verbose;
  bool  m_keep_blanks;
  bool  m_keep_tag_line;
  bool  m_first_line_only;
};

#endif

