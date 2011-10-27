/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogScanner.h                                        */
/*    DATE: June 3rd, 2008                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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



