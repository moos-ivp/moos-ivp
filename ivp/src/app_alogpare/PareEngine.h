/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PareEngine.h                                         */
/*    DATE: December 25th, 2015                                  */
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

#ifndef PARE_ENGINE_HEADER
#define PARE_ENGINE_HEADER

#include <vector>
#include <list>
#include <map>
#include <string>

class PareEngine
{
 public:
  PareEngine();
  ~PareEngine() {}

  bool setALogFileIn(std::string);
  bool setALogFileOut(std::string);
  bool addMarkVar(std::string);
  bool addHitListVar(std::string);
  bool addPareListVar(std::string);
  void defaultHitList();
  void defaultPareList();
  
  
  void setPareWindow(double v) {m_pare_window=v;}
  void pareFile();
  void printReport();

 protected:
  void passOneFindTimeStamps();
  void passTwoPareTimeStamps();
  void writeLine(FILE*, const std::string&) const;
  bool varOnHitList(std::string);
  bool varOnPareList(std::string);
  bool varOnList(std::vector<std::string>, std::string) const;
  
 protected:
  std::list<double>        m_timestamps;
  std::vector<std::string> m_mark_vars;
  std::vector<std::string> m_hitlist_vars;
  std::vector<std::string> m_parelist_vars;

  std::map<std::string, bool>  m_hit_cache;
  std::map<std::string, bool>  m_pare_cache;
  
  std::string m_alog_file_in;
  std::string m_alog_file_out;

  double  m_pare_window;
};

#endif
