/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GenericSensor_Model.h                                */
/*    DATE: Jan 28th, 2012                                       */
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

#ifndef UFLD_GENERIC_SENSOR_MODEL_HEADER
#define UFLD_GENERIC_SENSOR_MODEL_HEADER

#include <vector>
#include <string>
#include <map>
#include "XYHazard.h"
#include "XYPolygon.h"
#include "VarDataPair.h"
#include "NodeRecord.h"

class GenericSensor_Model
{
 public:
  GenericSensor_Model();
  virtual ~GenericSensor_Model() {};

  bool  setParam(std::string param, std::string value);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string src_app, std::string src_community);
  void  iterate();
  void  setCurrTime(double);
  void  setTimeWarp(double v) {m_time_warp=v;};
  void  perhapsSeedRandom();
  void  sortSensorProperties();

  std::vector<VarDataPair> getMessages(bool clear=true);
  std::vector<VarDataPair> getVisuals();

 protected: // Configuration utility
  bool    addSourcePoint(std::string);
  bool    addSensorConfig(std::string);
  bool    setSensorTransparency(std::string);
  bool    setTermReportInterval(std::string);
  bool    setOptionsSummaryInterval(std::string);
  bool    setMinResetInterval(std::string);
  bool    setMinSensorInterval(std::string);

 protected: // Incoming mail utility
  bool    handleNodeReport(const std::string&);
  bool    handleSensorRequest(const std::string&);
  bool    handleSensorConfig(const std::string&, const std::string&);

 protected: // Outgoing mail utility
  void    addMessage(const std::string&, const std::string&);
  void    addMessage(const std::string&, double);
  void    postSensorReport(double ptx, double pty, std::string vname);

 protected: // Utilities
  bool    setVehicleSensorSetting(std::string, double);
  bool    updateNodeRecords(NodeRecord);
  void    printReport();

  void    memo(const std::string&);
  void    memoErr(const std::string&);

 protected: // State variables
  double       m_curr_time;
  double       m_time_warp;
  double       m_last_report_time;
  double       m_last_summary_time;
  unsigned int m_iterations;
  unsigned int m_reports;

  // Map from vehicle name to latest node report;
  std::map<std::string, NodeRecord>     m_map_node_records;
  // Map from vehicle name to latest node report timestamp;
  std::map<std::string, double>         m_map_last_node_update;
  // Map from vehicle name to latest sensor request from the vehicle;
  std::map<std::string, double>         m_map_last_sensor_req;

  // Messages to be grabbed by the MOOSApp for posting to MOOSDB
  std::vector<VarDataPair>     m_messages;

  // Messages to be displayed to the terminal
  std::map<std::string, unsigned int>   m_map_memos;
  std::map<std::string, unsigned int>   m_map_err_memos;

 protected: // Configuration variables
  double      m_min_sensor_interval;
  double      m_min_reset_interval;
  double      m_term_report_interval;
  double      m_options_summary_interval;
  bool        m_seed_random;

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

  // Key for each map below is the vehicle name. 
  std::map<std::string, double>       m_map_reset_time;
  std::map<std::string, unsigned int> m_map_reset_total;  
  std::map<std::string, unsigned int> m_map_reports_total;  
  std::map<std::string, double>       m_map_vehicle_sensor_range;

  // Visual preferences
  bool        m_show_source_pts;
  std::string m_color_source_pts;
  double      m_scope_transparency;
};

#endif 

