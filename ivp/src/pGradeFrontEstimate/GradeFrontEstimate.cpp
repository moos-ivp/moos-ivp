/************************************************************/
/*    NAME:                                                 */
/*    ORGN: MIT                                             */
/*    FILE: GradeFrontEstimate.cpp                          */
/*    DATE: January 2012                                    */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GradeFrontEstimate.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;
#include <math.h>
//---------------------------------------------------------
// Constructor

GradeFrontEstimate::GradeFrontEstimate()
{

  m_curr_time  = 0;
  m_start_time  = 0;
  m_time_warp  = 1;
  m_reports    = 0;
  m_last_report_time  = 0;
  m_last_summary_time = 0;    // last time settings options summary posted

  m_start_time_local = 0;
  m_db_uptime  = 0;

  
  // Configuration variables
  m_term_report_interval = 0.8;      // realtime (non-timewarp) seconds
  // Front model parameters
  //------------------------------------------------------------- 
  m_offset = 200;
  m_angle = 20;
  m_amplitude = 50;
  m_period = 120;
  m_wavelength = 300;
  m_alpha = 100;  
  m_beta = 30;
  m_T_N = 20;
  m_T_S = 25;
  reported=false;
  estimate_report="";
  front_model_vars="";

}

//---------------------------------------------------------
// Destructor

GradeFrontEstimate::~GradeFrontEstimate()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GradeFrontEstimate::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  AppCastingMOOSApp::OnNewMail(NewMail);
  std::string old_front_model_vars=front_model_vars;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    double dval  = msg.GetDouble(); 
    if (key=="UCTD_PARAMETER_ESTIMATE" && !reported ){
      estimate_report=sval;
      setCurrTime(MOOSTime());
      reported=true;
    }
    if(key == "DB_UPTIME") {
      m_db_uptime = dval;
    }
    if(key == "DEPLOY_ALL") {
      if (sval == "true") {
	setStartTime(MOOSTime());
	cout << "Deploy, Start Time " << m_start_time << endl;
	reported = false;
      }
    }
    if (key == "UCTD_TRUE_PARAMETERS"){
      if (front_model_vars==""){
	front_model_vars=sval;
	vector<string> strvec = parseString(front_model_vars,",");
	for (unsigned int i=0;i<strvec.size();i++){
	  string cur_str=strvec[i];
	 
	  SetParam(parseString(cur_str,"=")[0],parseString(cur_str,"=")[1]);
	}
      }
    }
  }
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setCurrTIme

void GradeFrontEstimate::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: setStartTIme

void GradeFrontEstimate::setStartTime(double new_time)
{
  if(new_time > m_start_time)
    m_start_time = new_time;
  m_start_time_local = m_db_uptime;
}


bool GradeFrontEstimate::SetParam(string param, string value)
{
  bool handled = false;

  if (param == "offset")
    {
      m_offset = atof(value.c_str());
      handled = true;
      cout << "offset = " << m_offset << endl; 
    }
  else if (param == "angle")
    {
      m_angle = atof(value.c_str());
      handled = true;
      cout << "angle = " << m_angle << endl; 
    }
  else if (param == "amplitude")
    {
      m_amplitude = atof(value.c_str());
      handled = true;
      cout << "amplitude = " << m_amplitude << endl; 
    }
  else if (param == "period")
    {
      m_period = atof(value.c_str());
      handled = true;
      cout << "m_period = " << m_period << endl; 
    }
  else if (param == "wavelength")
    {
      m_wavelength = atof(value.c_str());
      handled = true;
      cout << "wavelength = " << m_wavelength << endl; 
    }
  else if (param == "alpha")
    {
      m_alpha = atof(value.c_str());
      handled = true;
      cout << "alpha = " << m_alpha << endl; 
    }
  else if (param == "beta")
    {
      m_beta = atof(value.c_str());
      handled = true;
      cout << "beta = " << m_beta << endl; 
    }
  else if (param == "tempnorth")
    {
      m_T_N = atof(value.c_str());
      handled = true;
      cout << "m_T_N = " << m_T_N << endl; 
    }
  else if (param == "tempsouth")
    {
      m_T_S = atof(value.c_str());
      handled = true;
      cout << "m_T_S = " << m_T_S << endl; 
    }

  return(handled);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GradeFrontEstimate::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GradeFrontEstimate::Iterate()
{
  AppCastingMOOSApp::Iterate();
  double warp_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = warp_elapsed_time;
  if(m_time_warp > 0)
    real_elapsed_time = warp_elapsed_time / m_time_warp;
  if(real_elapsed_time > m_term_report_interval) {
    //    printReport();
    m_last_report_time = m_curr_time;}
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GradeFrontEstimate::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      //no parameters right now for config...
    }
  }
  
  m_time_warp = GetMOOSTimeWarp();

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GradeFrontEstimate::RegisterVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UCTD_PARAMETER_ESTIMATE", 0);
  Register("UCTD_TRUE_PARAMETERS",0);
  Register("DEPLOY_ALL",0);
  Register("DB_UPTIME",0);
}

void GradeFrontEstimate::postSensingScore(string vname, double error, double score)
{
  // Get the sensor range

  string gt = "vname=" + vname 
    + ",error=" + doubleToString(error)
    + ",score=" + doubleToString(score);
   
  Notify("UCTD_SCORE_REPORT", gt);

}
//---------------------------------------------------------
// Procedure: handleSensingReport
//   Example: vname=alpha,amplitude=50,offset=-100,period=100,wavelength=190,...
//   Compares field parameters estimated by vname to ground truth

std::string GradeFrontEstimate::handleSensingReport(const string& request)
{
  // Part 1: Parse the string report

  string vname;
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  ostringstream s;
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
    else if (param == "offset")
      r_offset = atof(value.c_str());
    else if (param == "angle")
      r_angle = atof(value.c_str());
    else if (param == "amplitude")
      r_amplitude = atof(value.c_str());
    else if (param == "period")
      r_period = atof(value.c_str());
    else if (param == "wavelength")
      r_wavelength = atof(value.c_str());
    else if (param == "alpha")
      r_alpha = atof(value.c_str());
    else if (param == "beta")
      r_beta = atof(value.c_str());
    else if (param == "tempnorth")
      r_T_N = atof(value.c_str());
    else if (param == "tempsouth")
      r_T_S = atof(value.c_str());
  }

  // compute estimation error
  double ref_amp = m_amplitude;
  if (ref_amp < 1) ref_amp =  1;
  double ref_off = fabs(m_offset);
  if (ref_off < 1) ref_off =  1;
  double ref_ang = fabs(m_angle);
  if (ref_ang < 1) ref_ang =  1;
  double ref_per = m_period;
  if (ref_per < 1) ref_per =  1;
  double ref_wav = m_wavelength;
  if (ref_wav < 1) ref_wav =  1;
  double ref_alp = fabs(m_alpha);
  if (ref_alp < 1) ref_alp =  1;
  double ref_bet = m_beta;
  if (ref_bet < 1) ref_bet =  1;
  double ref_tn = m_T_N;
  if (ref_tn < 1) ref_tn =  1;
  double ref_ts = m_T_S;
  if (ref_ts < 1) ref_ts =  1;

  double error = pow((m_amplitude-r_amplitude)/ref_amp,2)
    + pow((m_offset-r_offset)/ref_off,2)
    + pow((m_angle-r_angle)/ref_ang,2)
    + pow((m_period-r_period)/ref_per,2)
    + pow((m_wavelength-r_wavelength)/ref_wav,2)
    + pow((m_alpha-r_alpha)/ref_alp,2)
    + pow((m_beta-r_beta)/ref_bet,2)
    + pow((m_T_N-r_T_N)/ref_tn,2)
    + pow((m_T_S-r_T_S)/ref_ts,2);
  error /= 9;
  error = sqrt(error);
  double score = 1/error;

  double time_threshold = 1200;
  double elapsed = m_curr_time - m_start_time;
  double time_fac = 1;
  if (elapsed > time_threshold)
    time_fac = elapsed/time_threshold;
  score = 1e2*score/time_fac;
  double time_limit = 1800;
  if (elapsed > time_limit)
    score = 0;

  s << "=================== " << endl;
  s << "Report from " << vname << endl ;
  s << "=================== " << endl;
  s << "Error        " << error << endl ;
  s << "Elapsed time " << m_curr_time-m_start_time << endl ;
  s << "Score        " << score << endl ;

  s << "Parameter estimates " << endl;
  s << "=================== " << endl;
  s << "Offset     " << r_offset << " Actual " << m_offset << endl; 
  s << "Angle      " << r_angle << " Actual " << m_angle << endl; 
  s << "Amplitude  " << r_amplitude << " Actual " << m_amplitude << endl; 
  s << "Period     " << r_period << " Actual " << m_period << endl; 
  s << "Wavelength " << r_wavelength << " Actual " << m_wavelength << endl; 
  s << "Alpha      " << r_alpha << " Actual " << m_alpha << endl; 
  s << "Beta       " << r_beta << " Actual " << m_beta << endl; 
  s << "Temp_North " << r_T_N << " Actual " << m_T_N << endl; 
  s << "Temp_South " << r_T_S << " Actual " << m_T_S << endl; 
  //post the score to moosdb:
  postSensingScore(vname,error, score);
  return(s.str());
}
bool GradeFrontEstimate::buildReport()
{
  m_msgs << "GradeFrontEstimate      " << endl;
  m_msgs << "------------------------" << endl;
  m_msgs << "Start time   " << m_start_time_local << endl ;
  std::string outstring;
  if (reported){
    
    outstring=handleSensingReport(estimate_report);
    msg_appcast=outstring;
    reported=false;
  }
  
  else if (estimate_report=="") {
    outstring= "waiting for an estimate report ...\n";
  }
  else {
    outstring=msg_appcast;
  }
  m_msgs << outstring;
  
  return(true);
}
