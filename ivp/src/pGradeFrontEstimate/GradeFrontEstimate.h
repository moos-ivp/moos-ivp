/************************************************************/
/*    NAME:                                                 */
/*    ORGN: MIT                                             */
/*    FILE: GradeFrontEstimate.h                            */
/*    DATE: January 2012                                    */
/************************************************************/

#ifndef GradeFrontEstimate_HEADER
#define GradeFrontEstimate_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
class GradeFrontEstimate : public AppCastingMOOSApp
{
 public:
   GradeFrontEstimate();
   ~GradeFrontEstimate();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();
   
   std::string    handleSensingReport(const std::string&);
   bool buildReport();
   void    postSensingScore(std::string vname, double error, double score);
   bool SetParam(std::string param,std::string value);
   void setCurrTime(double new_time);
   void setStartTime(double new_time);
 protected:
   std::string estimate_report;//what you get from UCTD_PARAMETER_ESTIMATE
   bool reported;
   std::string msg_appcast;//message for appcasting report
   std::string front_model_vars;//string from CTDSensor_MOOSApp that describes true values
   //Front true values: (from front_model_vars)
  
  double m_offset;
  double m_angle;
  double m_amplitude;
  double m_period;
  double m_wavelength;
  double m_alpha;
  double m_beta;
  double m_T_N;
  double m_T_S;

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
  //time variables
  double m_curr_time;
  double m_start_time;
  double m_time_warp;
  double m_last_report_time;
  double m_last_summary_time;
  double m_reports;
  double m_term_report_interval;

  // added by mikerb may1518
  double m_start_time_local;
  double m_db_uptime;
};

#endif 
