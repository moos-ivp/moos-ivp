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
  bool writeOutput();
  void addMacro(std::string, std::string, bool=false);
  void setForce(bool v) {m_force=v;};

 protected:
  void addNewLine(std::string);

  std::string containsMacro(std::string);

 private:
  std::vector<std::string> newlines;
  std::map<std::string, std::string> macros;
  std::string infile;
  std::string outfile;

  bool m_force;
};

#endif 
