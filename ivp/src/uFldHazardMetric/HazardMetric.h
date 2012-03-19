/*****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard    */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA      */
/*   FILE: HazardMetric.h                                        */
/*   DATE: March 12th, 2012                                      */
/*****************************************************************/

#ifndef HAZARD_METRIC_HEADER
#define HAZARD_METRIC_HEADER

#include <string>
#include <vector>
#include <map>
#include "MOOSLib.h"
#include "XYHazard.h"
#include "XYHazardSet.h"

class HazardMetric : public CMOOSApp
{
 public:
  HazardMetric();
  ~HazardMetric() {};
  
 protected:  // Common MOOS functions
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:  // Application specific functions
  bool    processHazardFile(std::string filename);
  bool    addHazardReport(std::string report);
  void    evaluateReports();
  void    evaluateReport(std::string source);
  void    printReport();

  std::string findReportForHazard(const XYHazard);

  
 private: // Configuration variables
  double  m_hazard_right;  // reward  for hazard reported as hazard
  double  m_hazard_wrong;  // penalty for hazard reported as benign
  double  m_benign_right;  // reward  for benign reported as benign
  double  m_benign_wrong;  // penalty for benign reported as hazard
  double  m_hazard_zilch;  // penalty for ignoring a hazard
  double  m_benign_zilch;  // penalty for ignoring a benign

  double  m_report_interval;

 private: // State variables
  unsigned int m_iterations;
  double       m_time_warp;
  double       m_curr_time;
  double       m_last_report_time;
  
  // Messages to be displayed to the terminal
  std::map<std::string, int>   m_map_memos;

  // Add a deadline???

  // Vector of N Hazards
  XYHazardSet                           m_hazards;

  std::map<std::string, XYHazardSet>    m_map_reports;
  std::map<std::string, double>         m_map_report_time;
  std::map<std::string, std::string>    m_map_report_score;
  std::map<std::string, unsigned int>   m_map_report_amt;

  // Keep track of bad configuration lines
  std::vector<std::string> m_bad_configs;
};

#endif 
