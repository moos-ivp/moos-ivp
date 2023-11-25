/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MHashReporter.h                                      */
/*    DATE: May 12th, 2023                                       */
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

#ifndef MHASH_REPORTER_HEADER
#define MHASH_REPORTER_HEADER

#include <map>
#include <string>

class MHashReporter
{
 public:
  MHashReporter();
  ~MHashReporter() {}

  bool setALogFile(std::string);
  void setTerse()          {m_terse=true;}
  void setReportMHash()    {m_report="mhash";}
  void setReportODist()    {m_report="odist";}
  void setReportDuration() {m_report="duration";}
  void setReportName()     {m_report="name";}
  void setReportUTC()      {m_report="utc";}
  void setReportXHash()    {m_report="xhash";}
  void setReportAll()      {m_report="all";}

  bool handle();

protected: 
  void reportVerbose();
  void reportMHash();
  void reportODist();
  void reportDuration();
  void reportName();
  void reportUTC();
  void reportXHash() const;
  void reportAll();

  std::string getXHash() const;
  std::string getMaxOdoMHash() const;
  
protected: // config vars
  std::string m_alog_file;
  std::string m_report;
  std::string m_mhash_in;
  bool        m_terse;
  
protected: // running state vars
  bool        m_osx_virgin;
  bool        m_osy_virgin;
  double      m_osx;
  double      m_osy;
  double      m_osx_prev;
  double      m_osy_prev;
  double      m_prev_time;
  double      m_curr_time;
  double      m_utc_start_time;
  std::string m_curr_mhash;
  std::string m_vname;

protected: // assessment vars
  double      m_odometry;
  std::map<std::string, double> m_hash_odo;
};

#endif

