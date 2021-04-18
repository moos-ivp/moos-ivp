/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_MOOSApp.h                               */
/*    DATE: Jan 28th, 2012                                       */
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

#ifndef UFLD_GENERIC_SENSOR_MOOSAPP_HEADER
#define UFLD_GENERIC_SENSOR_MOOSAPP_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"

class GenericSensor_MOOSApp : public AppCastingMOOSApp
{
 public:
  GenericSensor_MOOSApp();
  virtual ~GenericSensor_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool  OnNewMail(MOOSMSG_LIST &NewMail);
  bool  Iterate();
  bool  OnConnectToServer();
  bool  OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool  buildReport();

 protected:
  void  registerVariables();

 protected: // Configuration utility
  bool  handleConfigSourcePoint(std::string);
  bool  handleConfigSensorOption(std::string);
  bool  handleConfigSensorTransparency(std::string);
  bool  handleConfigTermReportInterval(std::string);
  bool  handleConfigOptionsSummaryInterval(std::string);
  bool  handleConfigMinResetInterval(std::string);
  bool  handleConfigMinSensorInterval(std::string);

 protected: // Incoming mail utility
  bool  handleMailNodeReport(const std::string&);
  bool  handleMailSensorRequest(const std::string&);
  bool  handleMailSensorConfig(const std::string&, const std::string&);

 protected: // Outgoing mail utility
  void  postSensorReport(double ptx, double pty, std::string vname);

 protected: // Utilities
  bool  setVehicleSensorSetting(std::string, double);
  bool  updateNodeRecords(NodeRecord);
  void  sortSensorProperties();
  void  postVisuals();

 protected: // State variables ---------------------------------------
  double       m_last_summary_time;
  unsigned int m_reports;

  // Map from vehicle name to latest node report;
  std::map<std::string, NodeRecord>     m_map_node_records;
  // Map from vehicle name to latest node report timestamp;
  std::map<std::string, double>         m_map_last_node_update;
  // Map from vehicle name to latest sensor request from the vehicle;
  std::map<std::string, double>         m_map_last_sensor_req;

  // Messages to be displayed to the terminal
  std::map<std::string, unsigned int>   m_map_memos;
  std::map<std::string, unsigned int>   m_map_err_memos;

  // Key for each map below is the vehicle name. 
  std::map<std::string, double>       m_map_reset_time;
  std::map<std::string, unsigned int> m_map_reset_total;  
  std::map<std::string, unsigned int> m_map_requests_total;  
  std::map<std::string, unsigned int> m_map_reports_total;  
  std::map<std::string, double>       m_map_vehicle_sensor_range;

 protected: // Configuration variables -----------------------------
  double      m_min_sensor_interval;
  double      m_min_reset_interval;
  double      m_term_report_interval;
  double      m_options_summary_interval;
  double      m_sensor_transparency;

  // A string describing available sensor propery settings
  std::string m_sensor_prop_summary;

  // Allowable sensor settings for any vehicle
  std::vector<double>      m_sensor_ranges_dbl;
  std::vector<std::string> m_sensor_ranges_str;

  // A set of source points
  std::vector<double>      m_source_pt_x;
  std::vector<double>      m_source_pt_y;
  std::vector<std::string> m_source_pt_label;

  // Visual preferences
  bool        m_show_source_pts;
  std::string m_color_source_pts;
  double      m_scope_transparency;
};

#endif 









