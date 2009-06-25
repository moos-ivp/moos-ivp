/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Expander.h                                           */
/*    DATE: May 28th, 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef EXPANDER_HEADER
#define EXPANDER_HEADER

#include <vector>
#include <string>
#include <map>

class Expander
{
 public:
  Expander(std::string, std::string);
  ~Expander() {};
  
  bool expand();
  bool verifyInfile();
  bool verifyInfile(const std::string& filename);
  bool writeOutput();
  void addMacro(std::string, std::string, bool=false);
  void setForce(bool v) {m_force=v;};
  void addPath(std::string);

 protected:
  std::vector<std::string> 
    expandFile(std::string filename,
	       std::map<std::string, std::string> macros,
	       std::vector<std::string> filenames, 
	       bool& result);
  
  bool applyMacrosToLine(std::string&, 
			 std::map<std::string, std::string>);

  std::string containsMacro(std::string);
  std::string findFileInPath(std::string);

  bool checkIfDef(std::string, std::map<std::string, std::string>);
  bool checkIfNDef(std::string, std::map<std::string, std::string>);

 private:
  std::vector<std::string> m_path;
  std::vector<std::string> m_newlines;

  // For preventing self-referencing include-loops
  std::vector<std::string>           m_initial_filenames;
  std::map<std::string, std::string> m_initial_macros;

  std::string m_infile;
  std::string m_outfile;

  bool m_force;
  int  m_max_subs_per_line;
};

#endif 
