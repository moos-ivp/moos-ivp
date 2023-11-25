/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Sayer.cpp                                            */
/*    DATE: May 20, 2013                                         */
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

#include <cstdlib>
#include <iterator>
#include "MBUtils.h"
#include "VoiceUtils.h"
#include "fileutil.h"
#include "ACTable.h"
#include "Sayer.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Sayer::Sayer()
{
  // Initialize configuration variables with default values
  m_default_voice        = ""; 
  m_default_rate         = 200;
  m_max_utter_queue_size = 10;
  m_min_utter_interval   = 1.0;  // seconds
  m_interval_policy      = "from_end";
  m_os_mode              = "both";
  m_volume               = 1; // normal volume
  
  // Initialize state variables
  m_last_utter_time  = 0;
  m_isay_filter      = "none";  // or ignore, or hold
  m_unhandled_audios = 0;

  m_total_received = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Sayer::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    string comm  = msg.GetCommunity();

    string utter_source = comm + ":" + msrc;
    
#if 0 // Keep these around just for template
    double dval  = msg.GetDouble();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = true;
    if(key == "SAY_MOOS") {
      m_total_received++;
      handled = addUtterance(sval, utter_source);
    }
    else if(key == "SAY_FILTER") {
      string val = tolower(sval);
      if((val=="none") || (val=="ignore") || (val=="hold"))
	m_isay_filter = val;
      else
	handled = false;
    }
    else if(key == "SAY_VOLUME")
      handled = handleSetVolume(sval);

    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      handled = false;

    if(!handled)
      reportRunWarning("Unhandled Mail: " + key + "=" + sval);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool Sayer::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool Sayer::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // Check if enough time has elapsed for allowing a new utterance
  double curr_time = MOOSTime();
  double time_warp = GetMOOSTimeWarp();
  double elapsed   = curr_time - m_last_utter_time;
  double interval  = (time_warp * m_min_utter_interval);
  if((m_isay_filter != "hold") && (elapsed > interval)) {
    bool uttered = sayUtterance();
    if(uttered)
      m_last_utter_time = MOOSTime();
  }

#if 0
  cout << "===============================================" << endl;
  cout << "Priority Queue Dump" << endl;
  cout << "===============================================" << endl;
  vector<Utterance> utters = m_utter_pqueue.getCopyAllEntries();
  for(unsigned int i=0; i<utters.size(); i++) {
    string pwt = doubleToString(utters[i].getPriority(), 8);
    string txt = utters[i].getText();
    cout << pwt << "   " << txt << endl;
  }
#endif
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool Sayer::OnStartUp()
{
  string directives = "must_have_moosblock=false";
  AppCastingMOOSApp::OnStartUpDirectives(directives);

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "default_voice") && isVoice(value)) {
      m_default_voice = value;
      handled = true;
    }
    else if((param == "default_rate") && isNumber(value)) {
      m_default_rate = atof(value.c_str());
      handled = true;
    }
    else if(param == "interval_policy") {
      string policy = tolower(value);
      if((policy == "from_start") || (policy == "from_end")) {
	m_interval_policy = policy;
	handled = true;
      }
    }
    else if(param == "os_mode") {
      string mode = tolower(value);
      if((mode == "osx") || (mode == "linux") || (mode == "both")) {
	m_os_mode = mode;
	handled = true;
      }
    }
    else if((param == "audio_dir") && (value != "")) {
      m_audio_dirs.push_back(value);
      handled = true;
    }
    else if(param == "volume")
      handled = handleSetVolume(value);
    else if((param == "min_utter_interval") && isNumber(value)) {
      m_min_utter_interval = atof(value.c_str());
      if(m_min_utter_interval < 0)
	m_min_utter_interval = 0;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  reverse(m_audio_dirs.begin(), m_audio_dirs.end());

  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void Sayer::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("SAY_MOOS", 0);
  Register("SAY_FILTER", 0);
  Register("SAY_VOLUME", 0);
}


//---------------------------------------------------------
// Procedure: addUtterance
//   Example: say={Hello Charlie!}, rate=200

bool Sayer::addUtterance(string utterance_str, string utter_source)
{
  reportEvent("Utter Rec'd:" + utterance_str);

  if(m_utter_pqueue.size() >= m_max_utter_queue_size) {
    reportRunWarning("Utterance Ignored: Max Queue Size Exceeded");
    return(false);
  }

  Utterance utter;
  // If the utterance doesn't provide its own source info, fill it in with
  // the noted MOOS community and Source provided from the MOOS message.
  if(utter.getSource() == "")
    utter.setSource(utter_source);
  bool ok = utter.initFromString(utterance_str);
  if(ok) {
    utter.setTimeReceived(MOOSTime());
    if(utter.isTopPriority())
      m_utter_pqueue.pushTop(utter);
    else
      m_utter_pqueue.push(utter);
  }
  return(ok);
}

//---------------------------------------------------------
// Procedure: handleSetVolume()
//      Note: AFPlay (used when playing wav or mp3 files) takes a
//            numerical value between 0 and 255. Zero is mute. 255
//            might damage your speakers. 1 is normal. Values
//            between [0,2] are allowed. Convenience arguments of
//            mute=0, soft=0.5, vsoft=0.2, and loud=1.5 are supported.
//      Note: The MacOS say command will accept volume adjustments with
//            $ say "[[volm 1.5]] Hello World". Same range of values
//            used for afplay

bool Sayer::handleSetVolume(string volume)
{
  if(volume == "mute")
    m_volume = 0;
  else if(volume == "vsoft")
    m_volume = 0.1;
  else if(volume == "soft")
    m_volume = 0.4;
  else if(volume == "normal")
    m_volume = 1.0;
  else if(volume == "loud")
    m_volume = 1.5;
  else if(volume == "vloud")
    m_volume = 2.0;
  else if(volume == "softer")
    m_volume -= 0.1;
  else if(volume == "louder")
    m_volume += 0.1;
  else if(isNumber(volume))
    m_volume = atof(volume.c_str());
  else
    return(false);

  // Clip the volume. In my experience, setting the volume super high
  // may blow past sensible limits normally in place for the computer,
  // perhaps damaging internal speakers.
  if(m_volume < 0)
    m_volume = 0;
  if(m_volume > 2)
    m_volume = 2;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: sayUtterance
//   Purpose: Process the top element in the queue

bool Sayer::sayUtterance()
{
  // Don't do anything if there is nothing in the queue
  if(m_utter_pqueue.size() == 0)
    return(false);

  // Get the top utterance and pop the list one element
  Utterance utter = m_utter_pqueue.pop();
  utter.setTimePosted(MOOSTime());

  // Add it to the history for building appcast report
  m_utter_history.push_back(utter);
  if(m_utter_history.size() > 10)
    m_utter_history.pop_front();

  if(m_isay_filter == "ignore")
    return(true);

  // Process the utterence
  string srce  = utter.getSource();
  string text  = utter.getText();
  string file  = utter.getFile();
  string cmd;
  //-------------------------------------------------
  // Case 1: Utterance is in the form of text
  //-------------------------------------------------
  if(text != "") {
    double rate  = utter.getRate();
    string voice = utter.getVoice();
    // Apply default values if not explicitly set in the utterance
    if(voice == "")
      voice = m_default_voice;
    if(rate == 0)
      rate = m_default_rate;
    
    string str_rate = doubleToStringX(rate, 1);
    if(text != "")
      reportEvent("Say:" + text);
    
    // Build the system command string (OSX)
    if((m_os_mode == "osx") || (m_os_mode == "both")) {
      cmd  = "say -r " + str_rate;
      if(voice != "")
	cmd += " -v " + voice;

      // Ex: $ say "[[volm 2]] Hello"
      if(m_volume != 1)
	text = "[[volm " + doubleToStringX(m_volume) + "]] " + text;

      cmd += " \"" + text + "\" ";
    }
    // Build the system command string (Linux)
    else if((m_os_mode == "linux") || (m_os_mode == "both")) {
      cmd = "espeak -s " + str_rate;
      if(voice != "")
	cmd += " -v " + voice;
      cmd += "--stdin \"" + text + "\" ";
    }

  }

  //-------------------------------------------------
  // Case 2: Utterance is in the form of a audio file
  //-------------------------------------------------
  else if(file != "") {
    // Try to find the audio file, first in the launch directory
    string found_file;
    if(isRegularFile("./"+file))
      found_file = "./"+file;
    
    // Then search in the list of provided audio directories
    unsigned int i, vsize = m_audio_dirs.size();
    for(i=0; (i<vsize) && (found_file == ""); i++) {
      string fpath = m_audio_dirs[i] + "/" + file;
      if(isRegularFile(fpath))
	found_file = fpath;
    }
    
    // If not found, report an event, not a warning
    if(found_file == "") {
      string src = "source=" + srce;
      if(srce == "")
	src = "unknown source";
      string event_str = "Unhandled audio=" + file + " from " + src;
      reportEvent(event_str);
      m_unhandled_audios++;
      return(false);
    }

    // Build the system command string (OSX)
    if((m_os_mode == "osx") || (m_os_mode == "both")) {
      cmd = "afplay -v " + doubleToString(m_volume);
      cmd += " " + found_file;
    }
    // Build the system command string (Linux)
    else if((m_os_mode == "linux") || (m_os_mode == "both"))
      cmd = "aplay " + found_file;
    
  }
  else {
    reportRunWarning("Unhandled Utterance");
    return(false);
  }

  // By adding an ampersand at the end of the command line, it runs the 
  // job in the background thus returning immediately.
  if(m_interval_policy == "from_start")
    cmd += " &";

  // We don't check the act on the result, but we get it anyway to avoid
  // a compiler warning.
  int result;
  Notify("ISAY_DEBUGA", cmd);
  result = system(cmd.c_str());
  Notify("ISAY_DEBUGB", cmd);

  if(result != 0) 
    cout << "Possible error in the iSay syscmd:" << cmd << endl;

  return(true);
}

//------------------------------------------------------------
// Procedure: buildReport()
//   Example:
// 
// Configuration Parameters:
// -------------------------
//     Default Voice: alex
//      Default Rate: 200
//   Max Utter Queue: 1000
//   Min Utter Inter: 1
//
// Status:
// -------------------------
//   Utter Queue Size: 0
//             Filter: none    (ignore, hold)
//
// Source           Time  Time  Utterance
//                  Recd  Post  Utterance
// --------         ----  ----------------------------------
// archie:pHelmIvP  3.22  15.1  Returning
// betty:pHelmIvP   14.2  14.1  Deployed


bool Sayer::buildReport() 
{
  m_msgs << "Configuration Parameters:" << endl;
  m_msgs << "-------------------------" << endl;
  m_msgs << "   Default Voice: " << m_default_voice      << endl;
  m_msgs << "    Default Rate: " << m_default_rate       << endl;
  m_msgs << " Max Utter Queue: " << m_max_utter_queue_size << endl;
  m_msgs << " Min Utter Inter: " << m_min_utter_interval << endl;
  m_msgs << " Interval Policy: " << m_interval_policy    << endl;

  m_msgs << " Total received: " << uintToString(m_total_received) << endl;

  
  
  unsigned int i, vsize = m_audio_dirs.size();
  for(i=0; (i<vsize) && (i<3); i++) 
    m_msgs << "       Audio Dir: " << m_audio_dirs[i] << endl;
  if(i >= 3)
    m_msgs << "       Audio Dir: " << (vsize-3) << " others" << endl;

  m_msgs << endl;
  m_msgs << "Status:" << endl;
  m_msgs << "-------------------------" << endl;
  m_msgs << "  Utter Queue Size: " << m_utter_pqueue.size() << endl;
  m_msgs << "  Unhandled Audios: " << m_unhandled_audios    << endl;
  m_msgs << "            Filter: " << m_isay_filter         << endl;
  m_msgs << endl;


  ACTable actab(4);
  actab << "Source | Time | Time | Utterance";
  actab << "       | Recd | Post |          ";
  actab.addHeaderLines();
  list<Utterance>::reverse_iterator p;
  for(p=m_utter_history.rbegin(); p!=m_utter_history.rend(); p++) {
    Utterance utter = *p;
    string source = utter.getSource();
    if(source == "")
      source = "-";
    double time_recd = utter.getTimeReceived();
    double time_post = utter.getTimePosted();
    string time_recd_s = doubleToString((time_recd - m_start_time), 1);
    string time_post_s = doubleToString((time_post - m_start_time), 1);
    string text      = utter.getText();
    if(text == "")
      text = utter.getFile();
    if(text == "")
      text = "-";
    actab << source << time_recd_s << time_post_s << text;
  }	

  m_msgs << actab.getFormattedString();

  return(true);
}






