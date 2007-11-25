/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogClipper.h                                         */
/*    DATE: Nov 5th, 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef LOG_CLIPPER_HEADER
#define LOG_CLIPPER_HEADER

#include <vector>
#include <string>

class LogClipper
{
 public:
  LogClipper(std::string, std::string);
  ~LogClipper() {};
  
  bool clip(double mint, double maxt=0);
  bool verify_infile();
  bool write_output();

 private:
  std::string infile;
  std::string outfile;
  std::vector<std::string> newlines;
};

#endif 





