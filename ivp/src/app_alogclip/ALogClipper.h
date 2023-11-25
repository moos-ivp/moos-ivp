/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ALogClipper.h                                        */
/*    DATE: June 5th, 2008                                       */
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

#ifndef ALOG_CLIPPER_HEADER
#define ALOG_CLIPPER_HEADER

#include <string>
#include <vector>

class ALogClipper
{
 public:
  ALogClipper();
  ~ALogClipper() {}
  
  bool         openALogFileRead(std::string filename);
  bool         openALogFileWrite(std::string filename);
  unsigned int clip(double mintime, double maxtime);

  unsigned int getDetails(const std::string& statevar);

 protected:
  std::string getNextLine();
  bool        writeNextLine(const std::string& output);

  unsigned int m_kept_chars;
  unsigned int m_clipped_chars_front;
  unsigned int m_clipped_chars_back;
  unsigned int m_kept_lines;
  unsigned int m_clipped_lines_front;
  unsigned int m_clipped_lines_back;

 private:
  FILE *m_infile;
  FILE *m_outfile;

  std::vector<std::string> m_preserve_vars;
};

#endif 





