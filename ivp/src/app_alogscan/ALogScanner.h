/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ALogScanner.h                                        */
/*    DATE: June 3rd, 2008                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ALOG_SCANNER_HEADER
#define ALOG_SCANNER_HEADER

#include <vector>
#include <map>
#include <string>

class ALogScanner
{
 public:
  ALogScanner();
  ~ALogScanner() {};

  bool openALogFile(std::string);
  
  bool scan();
  bool clip(double mint, double maxt=0);
  bool writeOutput();

  std::string getNextLine();

 protected:

 private:
  std::string m_infile;
  std::string m_outfile;

  std::vector<std::string>  m_var_names;
  std::vector<unsigned int> m_var_count;
  std::vector<unsigned int> m_var_chars;

  std::map<std::string, int>  vmap;
    
  FILE *m_file;
};

#endif 


