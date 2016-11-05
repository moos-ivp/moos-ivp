/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HdgHandler.cpp                                       */
/*    DATE: Oct 24th, 2016                                       */
/*****************************************************************/

#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "MBUtils.h"
#include "AngleUtils.h"
#include "HdgHandler.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Constructor

HdgHandler::HdgHandler()
{
  m_verbose    = false;
}

//--------------------------------------------------------
// Procedure: setALogFileIn()

bool HdgHandler::setALogFileIn(string alog_file)
{
  if(!okFileToRead(alog_file)) {
    cout << "Unable to read from: " << alog_file << endl;
    exit(1);
  }

  m_alog_file = alog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: setKLogFileOut()

bool HdgHandler::setKLogFileOut(string klog_file)
{
  if(!okFileToWrite(klog_file)) {
    cout << "Unable to write to: " << klog_file << endl;
    exit(1);
  }

  m_klog_file = klog_file;
  return(true);
}

//--------------------------------------------------------
// Procedure: processALogFile()

bool HdgHandler::processALogFile()
{
  FILE *alog_file_ptr = fopen(m_alog_file.c_str(), "r");
  if(!alog_file_ptr)
    return(false);

  if(m_verbose)
    cout << "Processing file: " << m_alog_file << endl;
  
  unsigned int line_count  = 0;
  while(1) {
    line_count++;
    string line_raw = getNextRawLine(alog_file_ptr);

    if(m_verbose) {
      if((line_count % 10000) == 0)
	cout << "+" << flush;
      if((line_count % 100000) == 0)
	cout << " (" << uintToCommaString(line_count) << ") lines" << endl;
    }
    
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    if(line_raw == "eof") 
      break;
    
    string var = getVarName(line_raw);
    if(var == "NAV_X") {
      string s_data = getDataEntry(line_raw);
      string s_time = getTimeStamp(line_raw);
      double d_data = atof(s_data.c_str());
      double d_time = atof(s_time.c_str());
      m_lplot_x.setValue(d_time, d_data);
    }      
    else if(var == "NAV_Y") {
      string s_data = getDataEntry(line_raw);
      string s_time = getTimeStamp(line_raw);
      double d_data = atof(s_data.c_str());
      double d_time = atof(s_time.c_str());
      m_lplot_y.setValue(d_time, d_data);
    }      
    else if(var == "NAV_HEADING") {
      string s_data = getDataEntry(line_raw);
      string s_time = getTimeStamp(line_raw);
      double d_data = atof(s_data.c_str());
      double d_time = atof(s_time.c_str());
      m_lplot_hdg.setValue(d_time, d_data);
    }
    else if(var == "NAV_SPEED") {
      string s_data = getDataEntry(line_raw);
      string s_time = getTimeStamp(line_raw);
      double d_data = atof(s_data.c_str());
      double d_time = atof(s_time.c_str());
      m_lplot_spd.setValue(d_time, d_data);
    }
  }

  if(m_verbose) {
    cout << endl;
    cout << uintToCommaString(line_count) << " lines total." << endl;

    cout << "X LogPlot size:   " << m_lplot_x.size() << endl; 
    cout << "Y LogPlot size:   " << m_lplot_y.size() << endl; 
    cout << "Hdg LogPlot size: " << m_lplot_hdg.size() << endl; 
  }
  
  if(alog_file_ptr)
    fclose(alog_file_ptr);

  return(true);
}


//--------------------------------------------------------
// Procedure: processHeadingDeltas()

bool HdgHandler::processHeadingDeltas()
{
  unsigned int psize = m_lplot_x.size();
  if(psize == 0)
    return(false);
  
  for(unsigned int i=1; i<psize-1; i++) {
    
    double t0 = m_lplot_x.getTimeByIndex(i-1);
    double x0 = m_lplot_x.getValueByIndex(i-1);
    double y0 = m_lplot_y.getValueByTime(t0, true);
    double h0 = m_lplot_hdg.getValueByTime(t0, true);

    double t1 = m_lplot_x.getTimeByIndex(i);
    double x1 = m_lplot_x.getValueByIndex(i);
    double y1 = m_lplot_y.getValueByTime(t1, true);

    double t2 = m_lplot_x.getTimeByIndex(i+1);
    double x2 = m_lplot_x.getValueByIndex(i+1);
    double y2 = m_lplot_y.getValueByTime(t2, true);
    
    double delta = 0;
    double dist  = hypot(x0-x1, y0-y1);    
    if(dist > 0.1) {
      
      double apparent_hdg_1 = relAng(x0, y0, x1, y1);
      double apparent_hdg_2 = relAng(x1, y1, x2, y2);
      double hdg_avg = headingAvg(apparent_hdg_1, apparent_hdg_2);
      //double hdg_avg = apparent_hdg_1;

      delta = hdg_avg - h0;
      if(delta < 0)
	delta = -delta;
      if(delta > 180)
	delta = 360 - delta;
    }
    
    m_lplot_delta.setValue(t0, delta);

    if(m_verbose)
      cout << doubleToString(t0,2) << ": " << delta << endl;
  }

  return(true);
}


//--------------------------------------------------------
// Procedure: writeKLogFile()

bool HdgHandler::writeKLogFile()
{
  if(m_klog_file == "") {
    cout << "No klog file provided." << endl;
    return(false);
  }

  if(m_lplot_delta.size() == 0) {
    cout << "Empty klog contents." << endl;
    return(false);
  }

  FILE *f = fopen(m_klog_file.c_str(), "w");
  if(!f) {
    cout << "Could not open " << m_klog_file << " for writing." << endl;
    return(false);
  }

  
  for(unsigned int i=0; i<m_lplot_delta.size(); i++) {

    double time  = m_lplot_delta.getTimeByIndex(i);
    double delta = m_lplot_delta.getValueByIndex(i);

    string time_str = doubleToString(time,3);
    time_str = padString(time_str, 16, false);
    string var_str = "HEADING_DELTA        ";
    string src_str = "aloghdg         ";
    string delta_str = doubleToString(delta,5);

    string line = time_str + var_str + src_str + delta_str;
  

    fprintf(f, "%s\n", line.c_str());
  }

  fclose(f);
  return(true);
}


//--------------------------------------------------------
// Procedure: filterOutSpdLowerThan()

bool HdgHandler::filterOutSpdLowerThan(double thresh)
{
  LogPlot new_lplot_x;
  LogPlot new_lplot_y;
  LogPlot new_lplot_hdg;
  LogPlot new_lplot_spd;
  LogPlot new_lplot_delta;

  for(unsigned int i=0; i<m_lplot_x.size(); i++) {
    double x = m_lplot_x.getValueByIndex(i);
    double t = m_lplot_x.getTimeByIndex(i);
    double y = m_lplot_y.getValueByTime(t);
    double hdg = m_lplot_hdg.getValueByTime(t);
    double spd = m_lplot_spd.getValueByTime(t);
    double delta = m_lplot_delta.getValueByTime(t);

    if(spd > thresh) {
      new_lplot_x.setValue(t, x);
      new_lplot_y.setValue(t, y);
      new_lplot_hdg.setValue(t, hdg);
      new_lplot_spd.setValue(t, spd);
      new_lplot_delta.setValue(t, delta);
    }
  }

  m_lplot_x = new_lplot_x;
  m_lplot_y = new_lplot_y;
  m_lplot_hdg = new_lplot_hdg;
  m_lplot_spd = new_lplot_spd;
  m_lplot_delta = new_lplot_delta;
  return(true);
}

//--------------------------------------------------------
// Procedure: writeReport()

bool HdgHandler::writeReport()
{
  LogPlot plot_01 = m_lplot_delta;
  LogPlot plot_05 = m_lplot_delta;
  LogPlot plot_10 = m_lplot_delta;

  plot_01.filterOutHighest(0.01);
  plot_05.filterOutHighest(0.05);
  plot_10.filterOutHighest(0.10);

  cout << "plotsize: " << m_lplot_delta.size() << endl;
  cout << "Delta Mean: " << m_lplot_delta.getMean() << endl;
  cout << "-------------------------------" << endl;
  cout << "plotsize: " << plot_01.size() << endl;
  cout << "Delta Mean (worst 1 pct removed): " << plot_01.getMean() << endl;
  cout << "-------------------------------" << endl;
  cout << "plotsize: " << plot_05.size() << endl;
  cout << "Delta Mean (worst 5 pct removed): " << plot_05.getMean() << endl;
  cout << "-------------------------------" << endl;
  cout << "plotsize: " << plot_10.size() << endl;
  cout << "Delta Mean (worst 10 pct removed): " << plot_10.getMean() << endl;
  return(true);
}


