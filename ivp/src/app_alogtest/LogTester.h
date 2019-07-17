/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogTester.h                                          */
/*    DATE: April 19, 2019                                       */
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

#ifndef LOG_CHECKER_HEADER
#define	LOG_CHECKER_HEADER

#include <vector>
#include <string>
#include "ALogEntry.h"
#include "LogUtils.h"
#include "LogTest.h"

const std::string m_timestamp = "ALOG_TIMESTAMP";

class LogTester {
 public:
  LogTester();
  ~LogTester();

  bool addTestFile(std::string test_file);
  bool setALogFile(std::string alog_file);
  bool setMarkFile(std::string mark_file);

  void setVerbose(bool v=true)   {m_verbose=v;}
  void setOverWrite(bool v=true) {m_overwrite=v;}

  bool test();
  void print() const;
  
protected:

  bool parseTestFiles();
  bool parseTestFile(std::string);  
  bool finish();
  
 protected:
  
  bool m_verbose;
  bool m_overwrite;

  std::string m_alog_file;
  std::string m_mark_file;
  
  FILE* m_fptr;
  
  std::vector<LogTest> m_tests;

  std::vector<std::string> m_test_files;
};

#endif	









