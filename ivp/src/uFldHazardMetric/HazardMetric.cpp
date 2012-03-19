/***************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard  */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA    */
/*   FILE: HazardMetric.cpp                                    */
/*   DATE: March 12th, 2012                                    */
/***************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ColorParse.h"
#include "FileBuffer.h"
#include "HazardMetric.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsHazardSet.h"

using namespace std;

//---------------------------------------------------------
// Constructor

HazardMetric::HazardMetric()
{
  // Initialized configuration variables
  // Default Reward Structure
  m_hazard_right     = 10;
  m_hazard_wrong     = -50;
  m_benign_right     = 5;
  m_benign_wrong     = -25;
  m_hazard_zilch     = -75;
  m_benign_zilch     = 0;
  m_report_interval  = 0.75;   // Terminal report interval seconds

  // Initialize state variables
  m_iterations       = 0;
  m_time_warp        = 1;
  m_curr_time        = 0;
  m_last_report_time = 0;      // Timestamp of last terminal report
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMetric::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    
    if(key == "HAZARD_REPORT")
      addHazardReport(sval);

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

bool HazardMetric::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMetric::Iterate()
{
  m_iterations++;
  m_curr_time = MOOSTime();

  double moos_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = moos_elapsed_time / m_time_warp;
  if(real_elapsed_time >= m_report_interval) {
    m_last_report_time = m_curr_time;
    printReport();
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMetric::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      bool handled = false;
      if((param == "SCORE_HAZARD_RIGHT") && isNumber(value)) {
	m_hazard_right = atof(value.c_str());
	handled = true;
      }
      else if((param == "SCORE_HAZARD_WRONG") && isNumber(value)) {
	m_hazard_wrong = atof(value.c_str());
	handled = true;
      }
      else if((param == "SCORE_BENIGN_RIGHT") && isNumber(value)) {
	m_benign_right = atof(value.c_str());
	handled = true;
      }
      else if((param == "SCORE_BENIGN_WRONG") && isNumber(value)) {
	m_benign_wrong = atof(value.c_str());
	handled = true;
      }
      else if((param == "SCORE_HAZARD_ZILCH") && isNumber(value)) {
	m_hazard_zilch = atof(value.c_str());
	handled = true;
      }
      else if((param == "SCORE_BENIGN_ZILCH") && isNumber(value)) {
	m_benign_zilch = atof(value.c_str());
	handled = true;
      }
      else if((param == "APPTICK") || (param=="COMMSTICK"))
        handled = true;
      
      else if(param == "HAZARD_FILE")
	handled = processHazardFile(value);
      
      if(!handled)
	m_bad_configs.push_back(original_line);
    }
  }
  
  m_time_warp = GetMOOSTimeWarp();

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void HazardMetric::RegisterVariables()
{
  m_Comms.Register("HAZARD_REPORT", 0);
}


//------------------------------------------------------------
// Procedure: processHazardFile

bool HazardMetric::processHazardFile(string filename)
{
  cout << "Processing Hazard File: " << filename << endl;

  vector<string> lines = fileBuffer(filename);
  unsigned int i, vsize = lines.size();
  cout << "Total lines: " << vsize << endl;
  if(vsize == 0)
    return(false);

  XYHazardSet hazard_set;

  unsigned int count = 0;
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(lines[i], '=');
    string right = lines[i];
    if(left == "source")
      hazard_set.setSource(right);
    else if(left == "hazard") {
      XYHazard new_hazard = string2Hazard(right);
      if(new_hazard.valid()) {
	hazard_set.addHazard(new_hazard);
	count++;
      }
      else {
	string bad_entry = "Bad Hazard file entry (" + filename + "): " + right;
	cout << bad_entry << endl;
	m_bad_configs.push_back(bad_entry);
      }
    }
  }
  
  string hazard_set_source = hazard_set.getSource();
  
  if(hazard_set_source == "") {
    string bad_entry = "Hazard file (" + filename + ") has no src info. Rejected";
    cout << bad_entry << endl;
    m_bad_configs.push_back(bad_entry);
    return(false);
  }

  cout << "Total hazards added: " << count << endl;
  if(hazard_set_source == "actual")
    m_hazards = hazard_set;
  else {
    m_map_reports[hazard_set_source]      = hazard_set;
    m_map_report_time[hazard_set_source]  = m_curr_time;
    m_map_report_score[hazard_set_source] = "n/a";
    m_map_report_amt[hazard_set_source]++;
  }


  return(true);
}


//------------------------------------------------------------
// Procedure: addHazardReport

bool HazardMetric::addHazardReport(string report_str)
{
  XYHazardSet hazard_set = string2HazardSet(report_str);
  
  string src = hazard_set.getSource();
  
  if(src == "") {
    string memo = "Rejected HAZARD_REPORT: Missing Source info";
    m_map_memos[memo]++;
    return(false);
  }
  
  if(src == "actual") {
    string memo = "Rejected HAZARD_REPORT: Source claims to be actual";
    m_map_memos[memo]++;
      return(false);
  }

  if(hazard_set.size() == 0) {
    string memo = "Warning: HAZARD_REPORT of size ZERO received";
    m_map_memos[memo]++;
  }

  m_map_reports[src]      = hazard_set;
  m_map_report_time[src]  = m_curr_time;
  m_map_report_score[src] = "n/a";
  m_map_report_amt[src]++;
  
  return(true);
}

//------------------------------------------------------------
// Procedure: evaluateReports

void HazardMetric::evaluateReports()
{
  map<string, XYHazardSet>::iterator p;
  for(p=m_map_reports.begin(); p!=m_map_reports.end(); p++) {
    string source = p->first;
    evaluateReport(source);
  }
}

//------------------------------------------------------------
// Procedure: evaluateReport

void HazardMetric::evaluateReport(string source)
{
  if(m_map_reports.count(source) == 0)
    return;

  XYHazardSet src_hset = m_map_reports[source];

  double total_score = 0;
  string report_eval;

  unsigned int i, vsize = m_hazards.size();
  for(i=0; i<vsize; i++) {
    double   iscore = 0;
    XYHazard ihaz   = m_hazards.getHazard(i);
    string   ilabel = ihaz.getLabel();
    string   itype  = ihaz.getType();

    string   eval = "label=" + ilabel + ",truth=" + itype;

    XYHazard src_haz = src_hset.findHazardByLabel(ilabel);
    if(src_haz.valid() == false) {
      eval += ",report=nothing";
      if(itype == "hazard") 
	iscore = m_hazard_zilch;
      else
	iscore = m_benign_zilch;
    }
    else {
      string src_haz_type = src_haz.getType();
      eval += ",report=" + src_haz_type;
      if(itype == "hazard") {
	if(src_haz_type == "hazard") 
	  iscore = m_hazard_right;
	else
	  iscore = m_hazard_wrong;
      }
      else {  // itype = benign
	if(src_haz_type == "hazard") 
	  iscore = m_benign_wrong;
	else
	  iscore = m_benign_right;
      }
      eval += ",score=" + doubleToStringX(iscore);
    }
    
    total_score += iscore;
    if(report_eval != "")
      report_eval += "#";
    report_eval += eval;
  }
  
  string summary = "vname=" + source;
  summary += ",total_score=" + doubleToStringX(total_score);
  
  m_map_report_score[source] = doubleToStringX(total_score);

  report_eval = summary + "#" + report_eval;

  m_Comms.Notify("REPORT_FULL_EVAL", report_eval);
  m_Comms.Notify("REPORT_FULL_EVAL_"+toupper(source), report_eval);

  m_Comms.Notify("REPORT_EVAL", summary);
  m_Comms.Notify("REPORT_EVAL_"+toupper(source), summary);


  // REPORT_FULL_EVAL = vname=archie,total_score=450 # 
  //                    label=34,truth=hazard,report=hazard,score=10  #
  //                    label=17,truth=hazard,report=benign,score=-50 #
  //                    label=91,truth=benign,report=nothing,score=0 

}



//------------------------------------------------------------
// Procedure: printReport

void HazardMetric::printReport() 
{
  // Part 1: Header
  cout << endl << endl << endl << endl << endl;
  cout << "======================================================" << endl;
  cout << "uFldHazardMetric Report (" << m_iterations << ")" << endl;

  // Part 2: Configuration Warnings
  unsigned int config_warnings = m_bad_configs.size();
  if(config_warnings != 0) {
    cout << termColor("red") << "MOOS File Configuration Errors (";
    cout << config_warnings << ")" << endl;
    for(unsigned i=0; i<config_warnings; i++) {
      cout << "  [" <<stripBlankEnds(m_bad_configs[i]) << "]" << endl;
    }
  }
  cout << termColor() << endl;

  // Part 3: Normal Status Output
  cout << "Received Reports:" << endl;
  cout << "----------------------------------" << endl;
  cout << "Vname     Reports     Score   Time" << endl;
  cout << "-------   -------     -----   ----" << endl;    
  map<string, string>::iterator p;
  for(p=m_map_report_score.begin(); p!=m_map_report_score.end(); p++) {
    string vname = p->first;
    string score = p->second;
    unsigned int amt = m_map_report_amt[vname];
    double time  = m_map_report_time[vname];
    cout << vname << "  " << amt << "   " << score << "   " << time << endl;
  }

  // Part 4: Memo Messages
  cout << endl;
  cout << "Internal Memos:                            " << endl;
  cout << "======================================     " << endl;
  map<string, int>::iterator p1;
  for(p1=m_map_memos.begin(); p1!=m_map_memos.end(); p1++) 
    cout << " (" << p1->second << ") " << p1->first  << endl;


}
