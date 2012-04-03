/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_Model.h                                 */
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

#ifndef UFLD_HAZARD_SENSOR_MODEL_HEADER
#define UFLD_HAZARD_SENSOR_MODEL_HEADER

#include <vector>
#include <string>
#include <list>
#include <map>
#include "XYHazard.h"
#include "XYPolygon.h"
#include "VarDataPair.h"
#include "NodeRecord.h"

class HazardSensor_Model
{
 public:
  HazardSensor_Model();
  virtual ~HazardSensor_Model() {};

  bool  setParam(std::string param, std::string value, unsigned int=0);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string src_app, std::string src_community);
  void  iterate();
  void  setCurrTime(double);
  void  setTimeWarp(double v) {m_time_warp=v;};
  void  perhapsSeedRandom();
  void  sortSensorProperties();

  std::vector<VarDataPair> getMessages(bool clear=true);
  std::vector<VarDataPair> getQueueMessages();
  std::vector<VarDataPair> getVisuals();

 protected: // Configuration utility
  bool    addHazard(std::string);
  bool    addSensorConfig(std::string);
  bool    setSwathLength(std::string);
  bool    setRandomNoiseAlgorithm(std::string);
  bool    setSwathTransparency(std::string);
  bool    setTermReportInterval(std::string);
  bool    setOptionsSummaryInterval(std::string);

 protected: // Incoming mail utility
  bool    handleNodeReport(const std::string&);
  bool    handleSensorRequest(const std::string&);
  bool    handleSensorClear(const std::string&);
  bool    handleClassifyRequest(const std::string&);
  bool    handleSensorConfig(const std::string&, const std::string&);

 protected: // Outgoing mail utility
  void    addMessage(const std::string&, const std::string&);
  void    addQueueMessage(const std::string&, const std::string&, const std::string&);
  void    addMessage(const std::string&, double);
  void    postHazardDetectionReport(std::string hlabel, std::string vname);
  void    postHazardClassifyReport(std::string hlabel, std::string vname);

 protected: // Utilities
  bool    updateVehicleHazardStatus(unsigned int vix, std::string);
  bool    setVehicleSensorSetting(std::string, double, double, bool v=false);
  bool    processHazardFile(std::string filename);
  bool    updateNodeRecords(NodeRecord);
  bool    updateNodePolygon(unsigned int);
  void    updateSwathGeometry();
  void    calcSwathGeometry(double, double&, double&);
  void    printReport();

  unsigned int sensorSwathCount(double, std::string vname);

 protected: // State variables
  double       m_curr_time;
  double       m_time_warp;
  double       m_last_report_time;
  double       m_last_summary_time;
  unsigned int m_reports;
  unsigned int m_iterations;

  // map of hazard-labels to hazards
  std::map<std::string, XYHazard>     m_map_hazards;
  std::map<std::string, unsigned int> m_map_hazard_hits;
  std::map<std::string, unsigned int> m_map_hazard_class_queries;

  // Map from tag=<vehiname_hazardindex> to Boolean 
  // True if hazard is currently within the sensor scope of the vehicle.
  std::map<std::string, bool>  m_map_hv_status;

  // Vector of vehicles. Index between vectors match to same vehicle.
  std::vector<NodeRecord>      m_node_records;
  std::vector<XYPolygon>       m_node_polygons;

  // Messages to be grabbed by the MOOSApp for posting to MOOSDB
  std::vector<VarDataPair>     m_messages;

  // Limited-Frequency msgs to be grabbed by the MOOSApp for posting to MOOSDB
  std::map<std::string, std::list<VarDataPair> >  m_map_msgs_queued;
  std::map<std::string, double>                   m_map_msg_last_queue_time;


  // Messages to be displayed to the terminal
  std::map<std::string, int>   m_map_memos;

 protected: // Configuration variables
  double      m_min_reset_interval;
  double      m_term_report_interval;
  double      m_options_summary_interval;
  bool        m_seed_random;
  bool        m_verbose;
  double      m_swath_len;  // Same for everyone, all settings.
  double      m_min_queue_msg_interval;

  // Key for each map below is the vehicle name. Swath width and PD are
  // requested by the user. The pfa and pc are determined from that.
  std::map<std::string, double>       m_map_swath_width;
  std::map<std::string, double>       m_map_prob_detect;
  std::map<std::string, double>       m_map_prob_false_alarm;
  std::map<std::string, double>       m_map_prob_classify;
  std::map<std::string, double>       m_map_reset_time;
  std::map<std::string, unsigned int> m_map_reset_total;  
  std::map<std::string, unsigned int> m_map_detections;

  // Absolute sensor characteristics. User may choose one of the
  // available widths. ROC curve and classification accuracy follow.
  std::vector<double>       m_sensor_prop_width;
  std::vector<double>       m_sensor_prop_exp;
  std::vector<double>       m_sensor_prop_class;
  std::vector<unsigned int> m_sensor_prop_max;
  std::string               m_sensor_prop_summary;

  // Info for adding random noise to the sensor report.
  std::string m_rn_algorithm;   // Empty string = no random noise
  double      m_rn_uniform_pct;

  // Visual preferences
  bool        m_show_hazards;
  bool        m_show_swath;
  double      m_circle_duration;
  std::string m_color_hazard;
  std::string m_color_benign;
  std::string m_shape_hazard;
  std::string m_shape_benign;
  double      m_width_hazard;
  double      m_width_benign;
  double      m_swath_transparency;
};

#endif 

