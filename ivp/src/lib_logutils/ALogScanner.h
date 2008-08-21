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
#include "ScanReport.h"

class ALogScanner
{
 public:
  ALogScanner() {m_file=0;};
  ~ALogScanner() {};

  bool       openALogFile(std::string);
  ScanReport scan();

  std::string getNextLine();

 private:
  FILE *m_file;
};

#endif 


