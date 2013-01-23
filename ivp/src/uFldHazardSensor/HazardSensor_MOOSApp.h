/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_MOOSApp.h                               */
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

#ifndef UFLD_HAZARD_SENSOR_MOOSAPP_HEADER
#define UFLD_HAZARD_SENSOR_MOOSAPP_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "XYPolygon.h"
#include "NodeRecord.h"
#include "XYHazard.h"
#include "HeadingHistory.h"

class HazardSensor_MOOSApp : public AppCastingMOOSApp
{
 public:
  HazardSensor_MOOSApp();
  virtual ~HazardSensor_MOOSApp() {};

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
  bool    updateNodePolygon(unsigned int, bool sensor_on=true);
  void    updateSwathGeometry();
  void    calcSwathGeometry(double, double&, double&);

  unsigned int sensorSwathCount(double, std::string vname);

 protected: // State variables
  double       m_last_summary_time;

  // map of hazard-labels to hazards
  std::map<std::string, XYHazard>     m_map_hazards;
  std::map<std::string, unsigned int> m_map_hazard_hits;
  std::map<std::string, unsigned int> m_map_hazard_class_queries;

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
  // requested by the user. The pfa and pc are determined from that.
  std::map<std::string, double>       m_map_swath_width;
  std::map<std::string, double>       m_map_prob_detect;
  std::map<std::string, double>       m_map_prob_false_alarm;
  std::map<std::string, double>       m_map_prob_classify;
  std::map<std::string, double>       m_map_reset_time;
  std::map<std::string, unsigned int> m_map_reset_total;  
  std::map<std::string, unsigned int> m_map_sensor_reqs;  
  std::map<std::string, unsigned int> m_map_detections;

 protected: // Configuration variables
  double      m_min_reset_interval;
  double      m_options_summary_interval;
  bool        m_seed_random;
  double      m_swath_len;  // Same for everyone, all settings.
  double      m_min_queue_msg_interval;
  double      m_sensor_max_turn_rate;

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


