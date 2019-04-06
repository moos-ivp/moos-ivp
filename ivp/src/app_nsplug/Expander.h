/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Expander.h                                           */
/*    DATE: May 28th, 2005                                       */
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

#ifndef EXPANDER_HEADER
#define EXPANDER_HEADER

#include <vector>
#include <string>
#include <map>

class Expander
{
 public:
  Expander(std::string infile="", std::string outfile="");
  ~Expander() {}
  
  void setInFile(std::string s)  {m_infile  = s;}
  void setOutFile(std::string s) {m_outfile = s;}

  bool expand();
  bool verifyInfile();
  bool verifyInfile(const std::string& filename);
  bool writeOutput();
  void addMacro(std::string, std::string, bool=false);
  void setForce(bool v)      {m_force=v;}
  void setStrict(bool v)     {m_strict=v;}
  void setPartialsOK(bool v) {m_partial_expand_ok=v;}
  void setTerminal(bool v)   {m_terminal=v;}
  void setInteractive(bool v) {m_interactive=v;}
  void addPath(std::string);

 protected:
  std::vector<std::string> 
    expandFile(std::string filename,
	       std::map<std::string, std::string>& macros,
	       std::vector<std::string> filenames, 
	       bool& result);
  
  bool applyMacrosToLine(std::string&, 
			 std::map<std::string, std::string>,
			 unsigned int line_num);

  std::string containsMacro(std::string);
  std::string findFileInPath(std::string);

  bool checkIfDef(std::string, std::map<std::string, std::string>, unsigned int);
  bool checkIfDefConj(std::string, std::map<std::string, std::string>);
  bool checkIfDefDisj(std::string, std::map<std::string, std::string>);

  bool checkIfNDef(std::string, std::map<std::string, std::string>);

  void currMode(std::string);
  void pushMode(std::string);
  bool popMode();
  std::string currMode();
  bool skipLines();

  bool modeStackContains(std::string);
    
  void printModeStack();

 private:
  std::vector<std::string> m_path;
  std::vector<std::string> m_newlines;

  // For preventing self-referencing include-loops
  std::vector<std::string>           m_initial_filenames;
  std::map<std::string, std::string> m_initial_macros;

  std::string m_infile;
  std::string m_outfile;

  // if m_strict, quit if undefined macros are found
  bool m_strict;
  bool m_terminal;
  bool m_force;
  int  m_max_subs_per_line;
  std::vector<std::string> m_pmode;

  bool m_partial_expand_ok;

  bool m_interactive;
};

#endif 
