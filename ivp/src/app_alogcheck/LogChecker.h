/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LogChecker.h                                         */
/*    DATE: July 29, 2010                                        */
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

#ifndef _LOGCHECKER_H
#define	_LOGCHECKER_H

#include <vector>
#include <string>
#include "ALogEntry.h"
#include "InfoBuffer.h"
#include "LogicCondition.h"
#include "LogUtils.h"

const std::string m_timestamp = "ALOG_TIMESTAMP";

class LogChecker {
 public:
  LogChecker();
  virtual ~LogChecker();
  
  virtual bool parseInputFile(std::string input_file);
  virtual bool check(std::string alogfile, std::string output_file = "");
  virtual bool addFlag(std::string value, std::vector<LogicCondition> &flags);
  virtual bool updateInfoBuffer(ALogEntry entry);
  virtual bool checkFlags(std::vector<LogicCondition> flags);
  
  // Inlined functions
  virtual void setVerbose(bool verbose) {m_verbose = verbose;}
  virtual bool getVerbose(void)         {return(m_verbose);}
  
  virtual bool addStartFlag(std::string str) {return(addFlag(str, m_start_flags));}
  virtual bool addEndFlag(std::string str)   {return(addFlag(str, m_end_flags));}
  virtual bool addFailFlag(std::string str)  {return(addFlag(str, m_fail_flags));}
  virtual bool addPassFlag(std::string str)  {return(addFlag(str, m_pass_flags));}
  
  virtual bool checkStartFlags() {return(checkFlags(m_start_flags));}
  virtual bool checkEndFlags()   {return(checkFlags(m_end_flags));}
  virtual bool checkFailFlags()  {return(checkFlags(m_fail_flags));}
  virtual bool checkPassFlags()  {return(checkFlags(m_pass_flags));}
    
 protected:
  
  bool m_verbose;
  bool m_overwrite_output;

  InfoBuffer* m_info_buffer;
  FILE* m_output_file;
  
  std::vector<LogicCondition> m_start_flags;
  std::vector<LogicCondition> m_end_flags;
  std::vector<LogicCondition> m_pass_flags;
  std::vector<LogicCondition> m_fail_flags;
  
  std::vector<std::string> m_vars;
};

#endif	










