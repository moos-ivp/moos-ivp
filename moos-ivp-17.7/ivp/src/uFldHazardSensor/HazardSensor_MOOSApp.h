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

#ifndef UFLD_HAZARD_SENSOR_MOOSAPP_HEADER
#define UFLD_HAZARD_SENSOR_MOOSAPP_HEADER

#include <list>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "NodeRecord.h"
#include "XYHazard.h"
#include "HeadingHistory.h"
#include "VarDataPair.h"
#include "ClassifyQueue.h"
#include <list>

class HazardSensor_MOOSApp : public AppCastingMOOSApp
{
 public:
  HazardSensor_MOOSApp();
  virtual ~HazardSensor_MOOSApp() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();

 protected:
  void registerVariables();
  void perhapsSeedRandom();
  void sortSensorProperties();
  void postVisuals();

 protected: // Configuration utility
  bool    addHazard(std::string);
  bool    addSensorConfig(std::string);
  bool    setSwathLength(std::string);
  bool    setRandomNoiseAlgorithm(std::string);
  bool    setSwathTransparency(std::string);
  bool    setOptionsSummaryInterval(std::string);

 protected: // Incoming mail utility
  bool    handleNodeReport(const std::string&);
  bool    handleSensorRequest(const std::string&);
  bool    handleSensorClear(const std::string&);
  bool    handleClassifyRequest(const std::string&);
  bool    handleSensorConfig(const std::string&, const std::string&);

 protected: // Outgoing mail utility
  void    processClassifyQueue();
  void    postHazardDetectionReport(std::string hlabel, unsigned int vix);
  void    postHazardDetectionAspect(std::string hlabel);

 protected: // Utilities
  bool    updateVehicleHazardStatus(unsigned int vix, std::string);
  bool    rollDetectionDiceNormal(unsigned int vix, std::string, double);
  bool    rollDetectionDiceAspect(unsigned int vix, std::string, double);

  bool    setVehicleSensorSetting(std::string, double, double, bool v=false);
  bool    setVehicleSensorSettingPD(std::string, double);
  bool    processHazardFile(std::string filename);
  bool    updateNodeRecords(NodeRecord);
  bool    updateNodePolygon(unsigned int, bool sensor_on=true);
  void    updateSwathGeometry();
  void    calcSwathGeometry(double, double&, double&);
  void    postConfigurationAck(std::string vname);

  bool    processSensorRequests();
  bool    processSensorRequest(std::string vname);

  unsigned int sensorSwathCount(double, std::string vname);

 protected: // State variables
  double       m_last_summary_time;

  // map of hazard-labels to hazards
  std::map<std::string, XYHazard>     m_map_hazards;
  std::map<std::string, unsigned int> m_map_hazard_class_queries;

  std::map<std::string, std::list<double> > m_map_hazard_passes;

  std::string  m_hazard_file;
  unsigned int m_hazard_file_hazard_cnt;
  unsigned int m_hazard_file_benign_cnt;
  
  // Map from tag=<vehiname_hazardindex> to Boolean 
  // True if hazard is currently within the sensor scope of the vehicle.
  std::map<std::string, bool>  m_map_hv_status;

  // Vector of vehicles. Index between vectors match to same vehicle.
  std::vector<NodeRecord>      m_node_records;
  std::vector<HeadingHistory>  m_node_hdg_hist;
  std::vector<XYPolygon>       m_node_polygons;

  // Key for each map below is the vehicle name. Swath width and PD are
  // requested by the user. The exp, pfa and pc are determined from that.
  std::map<std::string, double>       m_map_swath_width;
  std::map<std::string, double>       m_map_swath_roc_exp;
  std::map<std::string, double>       m_map_prob_detect;
  std::map<std::string, double>       m_map_prob_false_alarm;
  std::map<std::string, double>       m_map_prob_classify;
  std::map<std::string, double>       m_map_reset_swath_time;
  std::map<std::string, unsigned int> m_map_reset_swath_req;  
  std::map<std::string, unsigned int> m_map_reset_swath_total;  
  std::map<std::string, double>       m_map_reset_pd_time;
  std::map<std::string, unsigned int> m_map_reset_pd_total;  

  std::map<std::string, double>       m_map_last_sensor_request;

  std::map<std::string, unsigned int> m_map_haz_detect_chances;  
  std::map<std::string, unsigned int> m_map_ben_detect_chances;  
  std::map<std::string, unsigned int> m_map_haz_detect_reports;  
  std::map<std::string, unsigned int> m_map_ben_detect_reports;  

  std::map<std::string, unsigned int> m_map_sensor_reqs;  
  std::map<std::string, unsigned int> m_map_detections;
  std::map<std::string, unsigned int> m_map_classify_reqs;  
  std::map<std::string, unsigned int> m_map_classify_answ;

  // Limited-Frequency msgs to be posted to the MOOSDB
  std::map<std::string, ClassifyQueue>            m_map_classify_queue;
  std::map<std::string, double>                   m_map_classify_last_time;

 protected: // Configuration variables
  double      m_min_reset_interval;
  double      m_options_summary_interval;
  bool        m_seed_random;
  double      m_swath_len;  // Same for everyone, all settings.
  double      m_min_queue_msg_interval;
  double      m_sensor_max_turn_rate;
  bool        m_ignore_resemblances;
  double      m_max_vehicle_speed; // above which, sensor is off

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
  bool        m_show_pd;
  bool        m_show_pfa;
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









