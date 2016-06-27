/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FrontGridRender.cpp                                  */
/*    DATE: Apr 13th, 2012                                       */
/*****************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "FrontGridRender.h"
#include "XYFormatUtilsConvexGrid.h"

using namespace std;

//---------------------------------------------------------
// Constructor

FrontGridRender::FrontGridRender()
{
  m_curr_time  = 0;
  m_iterations = 0;
  m_timewarp   = 1;

  m_true_params_set  = 0;
  m_guess_params_set = 0;
  
  // Last time a grid was posted and how often do we post (realtime)
  m_last_grid_post_time = 0;
  m_grid_post_interval  = 0.15;

  // Set the default rendering values
  m_grid_truth_show = true;
  m_grid_guess_show = false;

  // The m_grid_*_active variables represent whether the last grid posting of
  // that type was made with active=true or not.
  m_grid_truth_active = false;
  m_grid_guess_active = false;

  // Last time terminal report generated and how often do we post (realtime)
  m_last_report_time = 0;
  m_report_interval  = 0.6;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool FrontGridRender::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    
    if(key == "UCTD_TRUE_PARAMETERS")
      handleCTDParameters(sval, "truth");
    else if(key == "UCTD_PARAMETER_ESTIMATE")
      handleCTDParameters(sval, "guess");
    else if(key == "FGR_SHOW") {
      sval = tolower(sval);
      m_grid_truth_show = false;
      m_grid_guess_show = false;
      if((sval == "truth") || (sval == "both"))
	m_grid_truth_show = true;
      if((sval == "guess") || (sval == "both"))
	m_grid_guess_show = true;
    }
    
#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool FrontGridRender::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool FrontGridRender::Iterate()
{
  AppCastingMOOSApp::Iterate();
  m_curr_time = MOOSTime();
  m_iterations++;

  double grid_moos_elapsed_time = m_curr_time - m_last_grid_post_time;
  double grid_real_elapsed_time = grid_moos_elapsed_time / m_timewarp;
  if(grid_real_elapsed_time >= m_grid_post_interval) {

    if(m_grid_truth_show) {
      updateGridTruth();
      postGridTruth(true);
    }
    else
      postGridTruth(false);

    if(m_grid_guess_show) {
      updateGridGuess();
      postGridGuess(true);
    }
    else
      postGridGuess(false);

    m_last_grid_post_time = m_curr_time;
  }

  double moos_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = moos_elapsed_time / m_timewarp;
  if(real_elapsed_time >= m_report_interval) 
    printReport();
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool FrontGridRender::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  string grid_config;

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "GRID_CONFIG") {
	unsigned int len = grid_config.length();
	if((len > 0) && (grid_config.at(len-1) != ','))
	  grid_config += ",";
	grid_config += value;
      }	
      if((param == "GRID_REFRESH_INTERVAL") && isNumber(value)) {
	m_grid_post_interval = atof(value.c_str());
      }	
      else if((param != "APPTICK") && (param != "COMMSTICK"))
	m_map_memos["Unhandle config param: " + param]++;

    }
  }
  
  m_grid_truth = string2ConvexGrid(grid_config);
  m_grid_guess = string2ConvexGrid(grid_config);
  unsigned int grid_elements = m_grid_truth.size();

  m_map_memos["Initial Grid Elements: " + uintToString(grid_elements)]++;
  
  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void FrontGridRender::RegisterVariables()
{
  Register("UCTD_TRUE_PARAMETERS", 0);
  Register("UCTD_PARAMETER_ESTIMATE", 0);
  Register("FGR_SHOW", 0);
  AppCastingMOOSApp::RegisterVariables();
}

//------------------------------------------------------------
// Procedure: handleCTDParameters
//      Note: Expected format
//       
//  UCTD_TRUE_PARAMETERS = "vname=shoreside,offset=-100,angle=50,amplitude=50,
//                          period=120,wavelength=300,alpha=100,beta=40,
//                          tempnorth=20,tempsouth=25"

bool FrontGridRender::handleCTDParameters(string str, string truth_or_guess)
{
  string s_vname;
  string s_offset;
  string s_angle;
  string s_amplitude;
  string s_period;
  string s_wavelength;
  string s_alpha;
  string s_beta; 
  string s_tnorth;
  string s_tsouth;
  
  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];

    if(param == "vname")
      s_vname = value;
    else if((param == "offset") && isNumber(value))
       s_offset = value;
    else if((param == "angle") && isNumber(value))
       s_angle = value;
    else if((param == "amplitude") && isNumber(value))
       s_amplitude = value;
    else if((param == "period") && isNumber(value))
       s_period = value;
    else if((param == "wavelength") && isNumber(value))
       s_wavelength = value;
    else if((param == "alpha") && isNumber(value))
       s_alpha = value;
    else if((param == "beta") && isNumber(value))
       s_beta = value;
    else if((param == "tempnorth") && isNumber(value))
       s_tnorth= value;
    else if((param == "tempsouth") && isNumber(value))
       s_tsouth = value;
 
  }   
  //if(s_vname != "shoreside")
  //  return(false);

  if(s_vname == "") {
    m_map_memos["Faulty CTD True Params: Missing field: vname"]++;
    return(false);
  }
  if(s_offset == "") {
    m_map_memos["Faulty CTD True Params: Missing field: offset"]++;
    return(false);
  }
  if(s_angle == "") {
    m_map_memos["Faulty CTD True Params: Missing field: angle"]++;
    return(false);
  }
  if(s_amplitude == "") {
    m_map_memos["Faulty CTD True Params: Missing field: amplitude"]++;
    return(false);
  }
  if(s_period == "") {
    m_map_memos["Faulty CTD True Params: Missing field: period"]++;
    return(false);
  }
  if(s_wavelength == "") {
    m_map_memos["Faulty CTD True Params: Missing field: wavelength"]++;
    return(false);
  }
  if(s_alpha == "") {
    m_map_memos["Faulty CTD True Params: Missing field: alpha"]++;
    return(false);
  }
  if(s_beta == "") {
    m_map_memos["Faulty CTD True Params: Missing field: beta"]++;
    return(false);
  }
  if(s_tnorth == "") {
    m_map_memos["Faulty CTD True Params: Missing field: tnorth"]++;
    return(false);
  }
  if(s_tsouth == "") {
    m_map_memos["Faulty CTD True Params: Missing field: tsouth"]++;
    return(false);
  }

  if(truth_or_guess == "truth") {
    m_true_params_set++;
    m_frontsim_truth.setVars(atof(s_offset.c_str()),
			     atof(s_angle.c_str()),
			     atof(s_amplitude.c_str()),
			     atof(s_period.c_str()),
			     atof(s_wavelength.c_str()),
			     atof(s_alpha.c_str()),
			     atof(s_beta.c_str()),
			     atof(s_tnorth.c_str()),
			     atof(s_tsouth.c_str()));
    m_map_memos["Successful CTD True-Parameter Set"]++;
  }  
  else {
    m_guess_params_set++;
    m_frontsim_guess.setVars(atof(s_offset.c_str()),
			     atof(s_angle.c_str()),
			     atof(s_amplitude.c_str()),
			     atof(s_period.c_str()),
			     atof(s_wavelength.c_str()),
			     atof(s_alpha.c_str()),
			     atof(s_beta.c_str()),
			     atof(s_tnorth.c_str()),
			     atof(s_tsouth.c_str()));
    m_map_memos["Successful CTD Guess-Parameter Set"]++;
  }

  return(true);
}

//------------------------------------------------------------
// Procedure: updateGridTruth()

void FrontGridRender::updateGridTruth()
{
  unsigned int i, gsize = m_grid_truth.size();
  for(i=0; i<gsize; i++) {
    double cx = m_grid_truth.getElement(i).getCenterX();
    double cy = m_grid_truth.getElement(i).getCenterY();

    double temp = m_frontsim_truth.tempFunction(m_curr_time, cx, cy);

    m_grid_truth.setVal(i, temp);
  }
}

//------------------------------------------------------------
// Procedure: updateGridGuess()

void FrontGridRender::updateGridGuess()
{
  unsigned int i, gsize = m_grid_guess.size();
  for(i=0; i<gsize; i++) {
    double cx = m_grid_guess.getElement(i).getCenterX();
    double cy = m_grid_guess.getElement(i).getCenterY();

    double temp = m_frontsim_guess.tempFunction(m_curr_time, cx, cy);

    m_grid_guess.setVal(i, temp);
  }
}

//------------------------------------------------------------
// Procedure: postGridTruth

void FrontGridRender::postGridTruth(bool active)
{
  // If setting inactive and already inactive, just ignore
  if(!active && !m_grid_truth_active)
    return;

  m_grid_truth.set_label("front_grid_truth");

  if(active) {
    m_grid_truth_active = true;
    m_grid_truth.set_active(true);
  }
  else {
    m_grid_truth_active = false;
    m_grid_truth.set_active(false);
  }

  string spec = m_grid_truth.get_spec();


  m_Comms.Notify("VIEW_GRID", spec);
}

//------------------------------------------------------------
// Procedure: postGridGuess

void FrontGridRender::postGridGuess(bool active)
{
  // If setting inactive and already inactive, just ignore
  if(!active && !m_grid_guess_active)
    return;

  if(active) {
    m_grid_guess_active = true;
    m_grid_guess.set_active(true);
  }
  else {
    m_grid_guess_active = false;
    m_grid_guess.set_active(false);
  }

  m_grid_guess.set_label("front_grid_guess");
  string spec = m_grid_guess.get_spec();
  m_Comms.Notify("VIEW_GRID", spec);
}


//------------------------------------------------------------
// Procedure: printReport

void FrontGridRender::printReport() 
{
  m_last_report_time = m_curr_time;
  // Part 1: Header
  cout << endl << endl << endl << endl << endl;
  cout << "======================================================" << endl;
  cout << "pFrontGridRender Report (" << m_iterations << ")" << endl;

  cout << termColor() << endl;
  cout << "Grid Size:       " << m_grid_truth.size() << endl;
  cout << "True-Params Set: " << m_true_params_set << endl;
  cout << "Guess-Params Set: " << m_guess_params_set << endl;
  
  cout << "Grid Truth Show: " << boolToString(m_grid_truth_show) << endl;
  cout << "Grid Guess Show: " << boolToString(m_grid_guess_show) << endl;

  // Part 2: Memo Messages
  cout << endl;
  cout << "Internal Memos:                            " << endl;
  cout << "======================================     " << endl;
  map<string, int>::iterator p1;
  for(p1=m_map_memos.begin(); p1!=m_map_memos.end(); p1++) 
    cout << " (" << p1->second << ") " << p1->first  << endl;


}

bool FrontGridRender::buildReport()
{
  // Part 1: Header                                                                                                                                                                                                                           
  m_msgs << endl << endl << endl << endl << endl;
  m_msgs << "======================================================" << endl;
  m_msgs << "pFrontGridRender Report (" << m_iterations << ")" << endl;

  m_msgs << termColor() << endl;
  m_msgs << "Grid Size:       " << m_grid_truth.size() << endl;
  m_msgs << "True-Params Set: " << m_true_params_set << endl;
  m_msgs << "Guess-Params Set: " << m_guess_params_set << endl;

  m_msgs << "Grid Truth Show: " << boolToString(m_grid_truth_show) << endl;
  m_msgs << "Grid Guess Show: " << boolToString(m_grid_guess_show) << endl;

  // Part 2: Memo Messages                                                                                                                                                                                                                    
  m_msgs << endl;
  m_msgs << "Internal Memos:                            " << endl;
  m_msgs << "======================================     " << endl;
  map<string, int>::iterator p1;
  for(p1=m_map_memos.begin(); p1!=m_map_memos.end(); p1++)
    m_msgs << " (" << p1->second << ") " << p1->first  << endl;

  return(true);
}
