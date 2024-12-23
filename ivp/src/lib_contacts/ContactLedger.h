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
#include "ColoredPoint.h"

class ContactLedger
{
public:
  ContactLedger(unsigned int hist_size=0);
  ~ContactLedger() {};

public: // General configuration
  void setGeodesy(CMOOSGeodesy geodesy);
  bool setGeodesy(double dlat, double dlon);
  
  void setCurrTimeUTC(double utc) {m_curr_utc=utc;}
  void extrapolate(double utc=0);
  void setActiveVName(std::string vname); 

public: // Managing, Handle Node Reports
  std::string processNodeReport(std::string report,
				std::string& whynot);
  std::string processNodeRecord(NodeRecord record,
				std::string& whynot);

  NodeRecord preCheckNodeReport(std::string report,
				std::string& whynot);
  bool       preCheckNodeRecord(NodeRecord& record,
				std::string& whynot);

  void clearNode(std::string vname);  
  void clearAllNodes();  

public: // Status checks
  bool isValid(std::string) const;
  bool isStale(std::string, double thresh) const;
  bool hasVName(std::string) const;
  bool hasVNameValid(std::string) const;
  bool hasVNameValidNotStale(std::string, double v=0) const;

public: // Record Getters
  double getX(std::string vname, bool extrap=true) const;
  double getY(std::string vname, bool extrap=true) const;
  double getSpeed(std::string vname) const;
  double getHeading(std::string vname) const;
  double getDepth(std::string vname) const;
  double getLat(std::string vname) const;
  double getLon(std::string vname) const;

  double getUTC(std::string vname) const;
  double getUTCAge(std::string vname) const;
  double getUTCReceived(std::string vname) const;
  double getUTCAgeReceived(std::string vname) const;

  std::string getGroup(std::string vname) const;
  std::string getType(std::string vname) const;
  std::string getSpec(std::string vname) const;
  std::string getActiveVName() const {return(m_active_vname);}

  CPList getVHist(std::string vname) const;
  
public: // Statistics Getters
  unsigned int size() const {return(m_map_records_rep.size());}
  unsigned int totalReports() const {return(m_total_reports);}
  unsigned int totalReportsValid() const {return(m_total_reports_valid);}
  unsigned int totalReports(std::string vname) const;
  std::string  getClosestVehicle(double x, double y) const;
  bool getWeightedCenter(double& x, double& y) const;
  
public:
  bool groupMatch(std::string vname1, std::string vname2) const;

  std::set<std::string>    getVNamesByGroup(std::string grp) const;
  std::vector<std::string> getVNames() const;
  std::vector<std::string> getVNamesStale(double thresh) const;
  std::vector<std::string> getReportSkews() const;
  std::vector<std::string> getReportLags() const;
  
  NodeRecord getRecord(std::string vname, bool extrap=true) const;

protected:
  void extrapolate(std::string vname);
  void updateLocalCoords();
  void updateLocalCoords(NodeRecord&);
  void updateGlobalCoords(NodeRecord&);
  
protected: // Config vars
  double m_stale_thresh;
  double m_extrap_thresh;

  unsigned int m_history_size;

  std::string m_active_vname;
    
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

  std::map<std::string, CPList> m_map_hist;
  
  double m_curr_utc;

  unsigned int m_total_reports;
  unsigned int m_total_reports_valid;
  
  CMOOSGeodesy m_geodesy;
  bool         m_geodesy_init;
  unsigned int m_geodesy_updates;
};

#endif 

