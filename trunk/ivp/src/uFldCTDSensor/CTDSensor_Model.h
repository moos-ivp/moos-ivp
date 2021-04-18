/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CTDSensor_Model.h                                */
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

#ifndef UFLD_CTD_SENSOR_MODEL_HEADER
#define UFLD_CTD_SENSOR_MODEL_HEADER

#include <vector>
#include <string>
#include <map>
#include "NodeRecord.h"
#include "VarDataPair.h"
#include "../lib_henrik_anneal/CFrontSim.h"

class CTDSensor_Model
{
 public:
  CTDSensor_Model();
  virtual ~CTDSensor_Model() {}

  bool  setParam(std::string param, std::string value);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string src_app, std::string src_community);
  void  iterate();
  void  setCurrTime(double);
  void  setStartTime(double);
  void  setTimeWarp(double v) {m_time_warp=v;}
  void  perhapsSeedRandom();

  std::vector<VarDataPair> getMessages(bool clear=true);
  std::vector<VarDataPair> getVisuals();

// Configuration utility
  bool    FieldModelConfig();
  bool    SensorConfig();
  std::string    postTrueParameters();
 
 protected: 
  bool    setTermReportInterval(std::string);

 protected: // Incoming mail utility
  bool    handleNodeReport(const std::string&);
  bool    handleSensorRequest(const std::string&);
 

 protected: // Outgoing mail utility
  void    addMessage(const std::string&, const std::string&);
  void    addMessage(const std::string&, double);
  void    postSensorReport(double ptx, double pty, std::string vname);
  void    postSensingScore(std::string vname, double error, double score);

 protected: // Utilities

  void    memo(const std::string&);
  void    memoErr(const std::string&);

 protected: // State variables
  double       m_curr_time;
  double       m_start_time;
  double       m_time_warp;
  double       m_last_report_time;
  double       m_last_summary_time;
  unsigned int m_iterations;
  unsigned int m_reports;
  bool vals_not_sent;
  // Front configuration parameters
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

 protected: // Front model Configuration variables

  CFrontSim front;

  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;
  double m_offset;
  double m_angle;
  double m_amplitude;
  double m_period;
  double m_wavelength;
  double m_alpha;
  double m_beta;
  double m_T_N;
  double m_T_S;
  double m_sigma;

  // reported values
  double r_offset;
  double r_angle;
  double r_amplitude;
  double r_period;
  double r_wavelength;
  double r_alpha;
  double r_beta;
  double r_T_N;
  double r_T_S;

  double      m_term_report_interval;

  // A string describing available sensor propery settings

  // Key for each map below is the vehicle name. 

  // Visual preferences

};

#endif 

