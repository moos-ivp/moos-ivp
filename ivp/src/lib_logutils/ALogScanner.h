/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogScanner.h                                        */
/*    DATE: June 3rd, 2008                                       */
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

#ifndef ALOG_SCANNER_HEADER
#define ALOG_SCANNER_HEADER

#include <vector>
#include <map>
#include <string>
#include "ScanReport.h"

class ALogScanner
{
 public:
  ALogScanner() {m_file=0; m_use_full_source=true;}
  ~ALogScanner() {}

  bool       openALogFile(std::string);
  ScanReport scan();
  ScanReport scanRateOnly();

  void  setUseFullSource(bool v) {m_use_full_source=v;}
  
 private:
  FILE    *m_file;
  bool     m_use_full_source;

};

#endif 











