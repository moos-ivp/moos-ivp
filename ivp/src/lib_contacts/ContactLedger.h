/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactLedger.h                                      */
/*    DATE: Aug 31st 2022                                        */
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

#ifndef CONTACT_LEDGER_HEADER
#define CONTACT_LEDGER_HEADER

#include <set>
#include <string>
#include <vector>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "NodeRecord.h"

class ContactLedger
{
public:
  ContactLedger();
  ~ContactLedger() {};

  void setGeodesy(CMOOSGeodesy geodesy);
  bool setGeodesy(double dlat, double dlon);
  bool addIgnoreVName(std::string vname);
  bool addIgnoreGroup(std::string group);
  
  void setCurrTimeUTC(double utc) {m_curr_utc=utc;}
  void extrapolate(double utc=0);

  std::string processNodeReport(std::string report,
				std::string& whynot);
  std::string processNodeRecord(NodeRecord record,
				std::string& whynot);

  NodeRecord preCheckNodeReport(std::string report,
				std::string& whynot);
  bool preCheckNodeRecord(NodeRecord& record,
			  std::string& whynot);

  bool checkNodeRecord(NodeRecord record);
  
  void clearNode(std::string vname);  

  bool isValid(std::string) const;
  bool isStale(std::string, double thresh) const;
  bool hasVName(std::string) const;
  bool hasVNameValid(std::string) const;
  bool hasVNameValidNotStale(std::string, double v=0) const;

  double getX(std::string vname, bool extrap=true) const;
  double getY(std::string vname, bool extrap=true) const;
  double getSpeed(std::string vname) const;
  double getHeading(std::string vname) const;
  double getDepth(std::string vname) const;
  double getLat(std::string vname) const;
  double getLon(std::string vname) const;
  double getAge(std::string vname) const;
  double getAgeReceived(std::string vname) const;

  std::string getGroup(std::string vname) const;
  std::string getType(std::string vname) const;
  std::string getSpec(std::string vname) const;
  std::string getIgnoreVNames() const;
  std::string getIgnoreGroups() const;
  
  unsigned int size() const {return(m_map_records_rep.size());}
  unsigned int totalReports() const {return(m_total_reports);}
  unsigned int totalReportsValid() const {return(m_total_reports_valid);}
  unsigned int totalReports(std::string vname) const;

  bool groupMatch(std::string vname1, std::string vname2) const;

  std::set<std::string> getVNamesByGroup(std::string group) const;
  
  std::vector<std::string> getVNames() const;
  std::vector<std::string> getVNamesStale(double thresh) const;

  std::vector<std::string> getReportSkews() const;
  std::vector<std::string> getReportLags() const;
  
protected:
  NodeRecord getRecord(std::string vname, bool extrap=true) const;
  void       extrapolate(std::string vname);
  void       updateLocalCoords();
  void       updateLocalCoords(NodeRecord&);
  void       updateGlobalCoords(NodeRecord&);
  
protected: // Config vars
  double m_stale_thresh;
  double m_extrap_thresh;

  std::set<std::string> m_ignore_vnames;
  std::set<std::string> m_ignore_groups;
  
protected: // State vars
  // All keys are vnames for incoming node reports
  std::map<std::string, NodeRecord> m_map_records_rep;
  std::map<std::string, NodeRecord> m_map_records_ext;
  std::map<std::string, double>     m_map_records_utc; // loc time rcvd

  std::map<std::string, double>       m_map_records_total;
  std::map<std::string, double>       m_map_skew_min;
  std::map<std::string, double>       m_map_skew_max;
  std::map<std::string, unsigned int> m_map_skew_cnt;
  std::map<std::string, double>       m_map_skew_avg;
  
  double m_curr_utc;

  unsigned int m_total_reports;
  unsigned int m_total_reports_valid;
  
  CMOOSGeodesy m_geodesy;
  bool         m_geodesy_init;
  unsigned int m_geodesy_updates;
};

#endif 

