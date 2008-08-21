/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScanHandler.h                                        */
/*    DATE: June 5th, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SCAN_HANDLER_HEADER
#define SCAN_HANDLER_HEADER

#include "ScanReport.h"

class ScanHandler
{
 public:
  ScanHandler() {};
  ~ScanHandler() {}

  void setParam(const std::string&, const std::string&);
  void handle(const std::string&);

  void appStatReport();

 protected:

  std::string m_sort_style;

  ScanReport  m_report;

};

#endif
