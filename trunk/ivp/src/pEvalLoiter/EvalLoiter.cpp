/**************************************************************/
/*    NAME: Michael Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA  */
/*    FILE: EvalLoiter.cpp                                    */
/*    DATE: January 6th, 2016                                 */
/**************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "EvalLoiter.h"
#include "XYFormatUtilsPoly.h"
#include "NodeRecordUtils.h"
#include "LogicUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

EvalLoiter::EvalLoiter()
{
  //================================================
  // Init state variables
  //================================================
  m_loiter_mode = "transit";  // or "loiter" 

  // Expected to be overridden by mail. Speed at which transiting
  // is expected to occur, against which efficiency is measured.
  m_transit_speed = 1;

  m_conditions_ok = true;
  m_info_buffer   = new InfoBuffer;
  
  m_curr_dist_to_poly = -1;
  m_cnt_bhv_settings  = 0;
  m_cnt_eval_requests = 0;
  m_cnt_evals_posted  = 0;
  m_cnt_node_reports  = 0;
  m_cnt_mode_changes  = 0;
  
  //================================================
  // Init config variables
  //================================================
  // Window of time from which evaluation summary will be made
  m_eval_window = 30;  // seconds

  // Amount of time before we regard our own nav position unreliable.
  m_nav_stale_thresh = 10;  // seconds  

  // Range from poly where mode switches between transiting/loitering
  m_mode_thresh = 10;  // meters

  // Amount of position history needed to keep around for evaluating
  // transit efficiency.
  m_max_nav_age = 15;  // seconds

  // Range from poly where inefficiency rating is worst
  m_range_max_ineff = 50;  // meters

  // CPA threshold below which the encounter is considered notable
  m_notable_cpa_thresh = 0;

  // Efficiency threshold below which the encounter is considered notable
  m_notable_eff_thresh = 100;

  // Variable published indicating a notable encounter
  m_notable_cpa_var = "NOTABLE_CPA";

  // Variable published indicating a notable encounter
  m_notable_eff_var = "NOTABLE_EFF";
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool EvalLoiter::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key  = msg.GetKey();
    string sval = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = false;
    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
      handled = handleMailNodeReport(sval);
    else if(key == "EVAL_LOITER_REQ")
      handled = handleMailEvalRequest(sval);
    else if(key == "BHV_SETTINGS")
      handled = handleMailLoiterSettings(sval);
    else if(key == "APPCAST_REQ")
      handled = true;
    else {
      updateInfoBuffer(msg);
      handled = true;
    }
      
    if(!handled)
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool EvalLoiter::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool EvalLoiter::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Part 1: Check if minimal info on ownship and poly is valid.
  if((m_records.size() == 0) || !m_loiter_poly.is_convex()) {
    AppCastingMOOSApp::PostReport();
    return(true);
  }
  
  m_conditions_ok = checkConditions();
  if(!m_conditions_ok) {
    m_point_evals.clear();
    m_point_times.clear();
    m_records.clear();
    m_map_eval_requests.clear();
    AppCastingMOOSApp::PostReport();
    return(true);
  }
  
  // Part 2: Always need curr_dist_to_poly. If no node records, done.
  double curr_osx  = m_records.front().getX();
  double curr_osy  = m_records.front().getY();
  m_curr_dist_to_poly = m_loiter_poly.dist_to_poly(curr_osx, curr_osy);

  // Part 3: Always re-assess loiter mode before calculating point eval.
  checkLoiterMode();
  
  // Part 4: Always evaluate efficiency rating for this point in time.
  performPointEval();

  // Part 5: Check pending eval requests. Requests need to be old
  // enough before it is processed and removed. So this invocation
  // may result in nothing. But we need to check on every iteration.
  processEvalRequests();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool EvalLoiter::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "eval_window") 
      handled = setNonNegDoubleOnString(m_eval_window, value);
    else if(param == "notable_cpa_thresh") 
      handled = setNonNegDoubleOnString(m_notable_cpa_thresh, value);
    else if(param == "notable_eff_thresh") 
      handled = setNonNegDoubleOnString(m_notable_eff_thresh, value);
    else if(param == "notable_cpa_var") 
      handled = setNonWhiteVarOnString(m_notable_cpa_var, value);
    else if(param == "notable_eff_var") 
      handled = setNonWhiteVarOnString(m_notable_eff_var, value);
    else if(param == "nav_stale_thresh") 
      handled = setNonNegDoubleOnString(m_nav_stale_thresh, value);
    else if(param == "mode_thresh") 
      handled = setNonNegDoubleOnString(m_mode_thresh, value);
    else if(param == "max_nav_age") 
      handled = setNonNegDoubleOnString(m_max_nav_age, value);
    else if(param == "range_max_ineff") 
      handled = setNonNegDoubleOnString(m_range_max_ineff, value);
    else if(param == "condition") {
      LogicCondition new_condition;
      handled = new_condition.setCondition(value);
      if(handled)
	m_logic_conditions.push_back(new_condition);
      m_conditions_ok = false; // Assume false until shown otherwise
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  registerVariables();	
  return(true);
}


//---------------------------------------------------------
// Procedure: checkLoiterMode()

void EvalLoiter::checkLoiterMode()
{
  if(m_loiter_mode == "transit") {
    if(m_curr_dist_to_poly < (m_mode_thresh * 0.8)) {
      m_loiter_mode = "loiter";
      m_cnt_mode_changes++;
    }
  }
  else if(m_loiter_mode == "loiter") { 
    if(m_curr_dist_to_poly > (m_mode_thresh * 1.2)) {
      m_loiter_mode = "transit";
      m_cnt_mode_changes++;
    }
  }
  else 
    reportRunWarning("Unsupported mode: " + m_loiter_mode);
}

//---------------------------------------------------------
// Procedure: performPointEval()
//   Purpose: Make an evaluation of effeciency for this point
//            in time. In the case of transiting, it may ivolve
//            recent history data, but it is still regarded as
//            a point evaluation of efficiency.

void EvalLoiter::performPointEval()
{
  // Part 1: Sanity checks
  if(!m_loiter_poly.is_convex())
    return;
  if(m_records.size() == 0)
    return;

  double newest_timestamp = m_records.front().getTimeStamp();
  if((m_curr_time - newest_timestamp) > m_nav_stale_thresh)
    return;
  if((m_loiter_mode != "loiter") && (m_loiter_mode != "transit")) 
    return;

  // Part 3: Handle case where we're in the loiter mode
  if(m_loiter_mode == "loiter") {
    if(m_curr_dist_to_poly > m_range_max_ineff)
      m_curr_dist_to_poly = m_range_max_ineff;

    // Assuming m_range_max_ineff is a positive number, then 
    // percentage guaranteed to be in the range [0,1]. Zero
    // when super efficient, and 1 when super inefficient. 
    double pct = m_curr_dist_to_poly / m_range_max_ineff;

    // Efficiency score in the range of [0,100]. 100 when we are
    // super efficient
    double efficiency_score = (1-pct) * 100;
    addPointEval(efficiency_score);
  }
  // Part 4: Handle case where we're in the transit_mode
  else {
    double secs_ago  = m_max_nav_age;
    double prev_dist = getDistToPolyAtTime(secs_ago);

    // example: expected: 10, actual 12  --> 100%
    // example: expected: 10, actual 10  --> 100%
    // example: expected: 10, actual 5   --> 75%
    // example: expected: 10, actual 0   --> 50%
    // example: expected: 10, actual -5  --> 25%
    // example: expected: 10, actual -10 --> 0%
    // example: expected: 10, actual -12 --> 0%
    
    double expect_dist_closed = secs_ago * m_transit_speed;
    double actual_dist_closed = prev_dist - m_curr_dist_to_poly;
    
    double delta = expect_dist_closed - actual_dist_closed;

    if(delta < (-expect_dist_closed))
       delta = -expect_dist_closed;
    if(delta > (expect_dist_closed))
       delta = expect_dist_closed;
       
    // value range is twice the expected closing distance since
    // it's possible the vehicle may have been force on a trajectory
    // opening range during this whole time. The range coulde be
    // even greater if ownship exceeds the transit speed, but we'll
    // work with this value range.
    double value_range = 2 * expect_dist_closed;

    //cout << "curr_dist_to_poly: " << m_curr_dist_to_poly << endl;
    //cout << "expect_dist_closed: " << expect_dist_closed << endl;
    //cout << "actual_dist_closed: " << actual_dist_closed << endl;
    //cout << "delta:              " << delta << endl;
    //cout << "value_range:        " << value_range << endl;
    
    // As an edge case we'll say we were 100% efficient if the
    // transit speed is for some reason zero. We check for this as
    // a special case also since it would result in div by zero
    double efficiency_score = 100;
    if(value_range > 0) {
      double pct = (actual_dist_closed + expect_dist_closed) / value_range;
      //cout << "pct                 " << pct << endl << endl;
      efficiency_score = pct * 100;
    }
    addPointEval(efficiency_score);
  }
}


//---------------------------------------------------------
// Procedure: getDistToPolyAtTime()
//   Purpose: Find the distance of ownship to the loiter polygon given
//            the number of given seconds in the past. The stored history
//            of node records may not match exactly the given query time
//            so we search through the list to find the one closest. If
//            the query time is way in the past, beyond our history length
//            the oldest historical record will be used.
//            
//   Returns: Two values:
//            - The distance to the polygon
//            - The actual time (secs prior to curr_time) when the
//              distance was calculated. This is returned by overwriting
//              the given time argument.

double EvalLoiter::getDistToPolyAtTime(double& given_secs_ago) const
{
  // Sanity checks
  if(m_records.size() == 0)
    return(-1);
  if(given_secs_ago < 0)
    given_secs_ago = 0;

  // Get actual absolute time at which we want to know distance.
  double query_time   = m_curr_time - given_secs_ago;
  
  double min_age_diff = -1;
  double min_age_navx = 0;
  double min_age_navy = 0;
  double min_age_time = 0;

  list<NodeRecord>::const_iterator p;
  for(p=m_records.begin(); p!=m_records.end(); p++) {
    NodeRecord record = *p;
    double record_timestamp = record.getTimeStamp();
    double age_diff = query_time - record_timestamp;
    if(age_diff < 0)
      age_diff *= -1;
    if((min_age_diff < 0) || (age_diff < min_age_diff)) {
      min_age_diff = age_diff;
      min_age_navx = record.getX();
      min_age_navy = record.getY();
      min_age_time = record_timestamp;
    }
  }

  // Using the timestamp of the closest record to the given time,
  // note the actual number of seconds in the past for this record.
  // And store it back in the given argument as a return value.
  double actual_secs_ago = m_curr_time - min_age_time;
  given_secs_ago = actual_secs_ago;
  
  double dist = m_loiter_poly.dist_to_poly(min_age_navx, min_age_navy);
  return(dist);
}

//---------------------------------------------------------
// Procedure: addPointEval()

void EvalLoiter::addPointEval(double eval)
{
  if(eval < 0)
    eval = 0;
  else if(eval > 100)
    eval = 100;
  
  m_point_evals.push_front(eval);
  m_point_times.push_front(m_curr_time);
  
  bool done = false;
  while(!done && (m_point_evals.size() != 0)) {
    double item_time = m_point_times.back();
    double item_age  = m_curr_time - item_time;
    if(item_age > m_eval_window) {
      m_point_evals.pop_back();
      m_point_times.pop_back();
    }
    else
      done = true;
  }

  Notify("EVAL_LOITER_PT_EVAL", eval);
}

//---------------------------------------------------------
// Procedure: handleMailNodeReport

bool EvalLoiter::handleMailNodeReport(string str)
{
  m_cnt_node_reports++;
  
  // Part 1: Build the record, check validity, push onto list
  NodeRecord record = string2NodeRecord(str);
  if(!record.valid()) {
    reportRunWarning("Bad NODE_REPORT: " + str);
    return(false);
  }
  m_records.push_front(record);
  
  // Part 2: Prune the list, limited by age of members
  bool done = false;
  while(!done && (m_records.size() != 0)) {
    double oldest_time = m_records.back().getTimeStamp();
    double age = m_curr_time - oldest_time;
    if(age > m_max_nav_age) 
      m_records.pop_back();
    else
      done = true;
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailEvalRequest
//   Example: EVAL_LOITER_REQ = "id=129,cpa=23.19,v2=deb"

bool EvalLoiter::handleMailEvalRequest(string str)
{
  m_cnt_eval_requests++;

  // PArt 1: Make sure that the ID is not empty, or already used.
  if((str == "") || (m_map_eval_requests.count(str) != 0)) {
    reportRunWarning("duplicate eval request: " + str);
    return(false);
  } 
  reportEvent("EVAL_LOITER_REQ: " + str);
  m_map_eval_requests[str] = m_curr_time;  

  // Part 2: Check if the encounter is notable wrt CPA distance
  string cpa_str = tokStringParse(str, "cpa", ',', '=');
  double cpa_dbl = atof(cpa_str.c_str());
  if(cpa_dbl < m_notable_cpa_thresh)
    Notify(m_notable_cpa_var, cpa_dbl);

    
  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailLoiterSettings
//   Purpose: Figure out what the speed the vehicle is nominally
//            shooting for when the loiter behavior is active.
//            Need to know this to calculate transit efficiency
//            later on.
//   Example: BHV_SETTINGS = type=BHV_Loiter,name=loiter,x=19.41,y=-82.85,
//            clockwise=false,dynamic_clockwise=false,center_activate=false,
//         -->desired_speed=1.3,polygon=pts={11.4,-64.8:1.4,-74.8:
//            1.4,-90.8:11.4,-100.8:27.4,-100.8:37.4,-90.8:37.4,-74.8:
//            27.4,-64.8},label=henry_LOITER 

bool EvalLoiter::handleMailLoiterSettings(string str)
{
  // Keep candidate replacements to use only if whole string is ok
  double new_transit_speed = 0;
  XYPolygon new_poly;
  string bhv_type;
  string poly_str;
  
  vector<string> svector = parseStringQ(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    
    if((param == "desired_speed") && isNumber(value))
      new_transit_speed = atof(value.c_str());
    else if(param == "polygon") {
      new_poly = string2Poly(stripQuotes(value));
      poly_str = value;
    }
    else if(param == "type")
      bhv_type = value;
  }

  if(bhv_type != "BHV_Loiter")
    return(true);

  m_cnt_bhv_settings++;
  if(new_transit_speed == 0) {
    reportRunWarning("loiter speed not found or zero in BHV_SETTINGS");
    return(false);
  }

  if(!new_poly.is_convex()) {
    reportRunWarning("No or non-convex poly found in BHV_SETTINGS");
    reportRunWarning("bad poly:[" + poly_str + "]");
    reportRunWarning("spec poly:[" + new_poly.get_spec() + "]");
    return(false);
  }
  
  m_loiter_poly   = new_poly; 
  m_transit_speed = new_transit_speed;

  m_loiter_mode = "transit";
  
  return(true);  
}


//---------------------------------------------------------
// Procedure: clearEvals()

void EvalLoiter::clearEvals()
{
  m_point_evals.clear();
  m_point_times.clear();
}

//-----------------------------------------------------------
// Procedure: checkconditions()
//   Purpose: Determine if all the logic conditions in the vector
//            of conditions is met, given the snapshot of variable
//            values in the info_buffer.

bool EvalLoiter::checkConditions()
{
  if(!m_info_buffer) 
    return(false);
  
  // Phase 1: get all the variable names from all present conditions.
  vector<string> condition_vars = getUniqueVars(m_logic_conditions);
  
  // Phase 2: get values of all variables from the info_buffer and 
  // propogate these values down to all the logic conditions.
  for(unsigned int i=0; i<condition_vars.size(); i++) {
    string varname = condition_vars[i];
    bool   ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);
    
    if(ok_s) {
      for(unsigned int j=0; j<m_logic_conditions.size(); j++)
	m_logic_conditions[j].setVarVal(varname, s_result);
    }

    if(ok_d) {
      for(unsigned int j=0; j<m_logic_conditions.size(); j++)
      m_logic_conditions[j].setVarVal(varname, d_result);
    }
  }

    
  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
    bool satisfied = m_logic_conditions[i].eval();
    if(!satisfied)
      return(false);
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: updateInfoBuffer()

bool EvalLoiter::updateInfoBuffer(CMOOSMsg &msg)
{
  string key = msg.GetKey();
  string sdata = msg.GetString();
  double ddata = msg.GetDouble();

  if(msg.IsDouble()) 
    return(m_info_buffer->setValue(key, ddata));
  else if(msg.IsString())
    return(m_info_buffer->setValue(key, sdata));

  return(false);
}

//---------------------------------------------------------
// Procedure: processEvalRquests
//   Purpose: Check all pending eval requests and check each
//            to see if it's been in the queue long enough.
//            If so, perform the summary eval and remove it
//            from the queue.

void EvalLoiter::processEvalRequests()
{
  vector<string> ids_to_erase;
  
  map<string,double>::iterator p;
  for(p=m_map_eval_requests.begin(); p!=m_map_eval_requests.end(); p++) {
    // Example key: "id=76,cpa=12.5,v2=deb"
    string key  = p->first;
    double time = p->second;
    double age  = m_curr_time - time;
    // Check if it is time to perform the summary evaluation
    if(age > (m_eval_window / 3)) {
      // Part 1: build the summary 
      double eval_summary = getEvalSummary();
      string msg = key + ",eff=" + doubleToString(eval_summary,2);
      msg += ",v1=" + m_host_community + ",type=eval_loiter";
      
      // Part 2: post the message
      reportEvent("EVAL_SUMMARY: " + msg);
      Notify("ENCOUNTER_SUMMARY", msg);
      Notify("ENCOUNTER_SUMMVAL", eval_summary);
      m_cnt_evals_posted++;

      // Part 3: Check if the encounter is notable wrt low efficiency
      if(eval_summary < m_notable_eff_thresh)
	Notify(m_notable_eff_var, eval_summary);
      
      // Part 4: remove the pending request for an eval
      ids_to_erase.push_back(key);
    }      
  }

  for(unsigned int i=0; i<ids_to_erase.size(); i++)
    m_map_eval_requests.erase(ids_to_erase[i]);

}

//---------------------------------------------------------
// Procedure: getEvalSummary()
//   Purpose: Get the average of all the point evaluations.
//      Note: Assume history length is managed elsewhere.

double EvalLoiter::getEvalSummary() const
{
  // Sanity check
  if(m_point_evals.size() == 0)
    return(0);
  
  double total = 0;
  list<double>::const_iterator p;
  for(p=m_point_evals.begin(); p!=m_point_evals.end(); p++)
    total += *p;

  double avg = total / (double)(m_point_evals.size());
  return(avg);
}

//---------------------------------------------------------
// Procedure: registerVariables

void EvalLoiter::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("EVAL_LOITER_REQ", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("BHV_SETTINGS", 0);

  //=======================================================
  // Register for variables used in the logic conditions
  //=======================================================
  // First get all the variable names from all present conditions.
  vector<string> all_vars;
  for(unsigned int i=0; i<m_logic_conditions.size(); i++) {
    vector<string> svector = m_logic_conditions[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);
  
  // Register for all variables found in all conditions.
  for(unsigned int i=0; i<all_vars.size(); i++)
    Register(all_vars[i], 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool EvalLoiter::buildReport() 
{
  string eval_window_str      = doubleToStringX(m_eval_window);
  string nav_stale_thresh_str = doubleToStringX(m_nav_stale_thresh);
  string mode_thresh_str      = doubleToStringX(m_mode_thresh);
  string max_nav_age_str      = doubleToStringX(m_max_nav_age);
  string range_max_ineff_str  = doubleToStringX(m_range_max_ineff);
  string notable_eff_str      = doubleToStringX(m_notable_eff_thresh);
  string notable_cpa_str      = doubleToStringX(m_notable_cpa_thresh);

  m_msgs << "========================================="  << endl;
  m_msgs << "       eval_window: " << eval_window_str     << endl;
  m_msgs << "  nav_stale_thresh: " << nav_stale_thresh_str << endl;
  m_msgs << "       mode_thresh: " << mode_thresh_str     << endl;
  m_msgs << "       max_nav_age: " << max_nav_age_str     << endl;
  m_msgs << "   range_max_ineff: " << range_max_ineff_str << endl;
  m_msgs << "notable_eff_thresh: " << notable_eff_str << endl;
  m_msgs << "notable_cpa_thresh: " << notable_cpa_str << endl;
  m_msgs << endl;
  m_msgs << " cnt_bhv_settings:  " << uintToString(m_cnt_bhv_settings) << endl;
  m_msgs << " cnt_eval_requests: " << uintToString(m_cnt_eval_requests) << endl;
  m_msgs << " cnt_evals_posted:  " << uintToString(m_cnt_evals_posted) << endl;
  m_msgs << " cnt_node_reports:  " << uintToString(m_cnt_node_reports) << endl;
  m_msgs << " cnt_mode_changes:  " << uintToString(m_cnt_mode_changes) << endl;
  m_msgs << endl;
  m_msgs << "    conditions_ok:  " << boolToString(m_conditions_ok) << endl;
  m_msgs << "      loiter_mode:  " << m_loiter_mode   << endl;
  m_msgs << "    transit_speed:  " << m_transit_speed << endl;
  m_msgs << endl;
  m_msgs << "Latest Point Evals (Newest to Oldest): (" << m_point_evals.size() << ")" << endl;

  unsigned int cnt = 0;
  list<double>::iterator p;
  for(p=m_point_evals.begin(); (p!=m_point_evals.end() && (cnt < 8)); p++) {
    m_msgs << "    " << doubleToString(*p, 2) << endl;
    cnt++;
  }

  return(true);
}
