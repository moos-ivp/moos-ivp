/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Parser.h                                             */
/*    DATE: May 31st, 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef LOGPARSER_HEADER
#define LOGPARSER_HEADER

#include <vector>
#include <string>

class Parser
{
 public:
  Parser(std::string, std::string, std::string);
  ~Parser() {};
  
  bool parse();
  bool verify_infile();
  bool write_output();

 private:
  std::vector<std::string> inputlines;
  std::vector<std::string> newlines;
  std::string infile;
  std::string outfile;
  std::string columns;
  std::vector<int> cindex;
  std::string polyline;
};

#endif 





