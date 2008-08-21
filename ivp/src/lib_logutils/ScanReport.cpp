/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ScanReport.cpp                                       */
/*    DATE: June 4th, 2008                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "ScanReport.h"

using namespace std;

//--------------------------------------------------------
// Procedure: addLine
//     Notes: 

void ScanReport::addLine(double timestamp, const string& varname,
			 const string& source, const string& value)
{
  int chars = value.length();

  map<string,int>::iterator p;
  p = m_vmap.find(varname);
  if(p != m_vmap.end()) {
    int index = p->second;
    m_var_last[index] = timestamp;
    if(!strContains(m_var_sources[index], source))
      m_var_sources[index] += ("," + source);
    m_var_lines[index]++;
    m_var_chars[index] += chars;
  }
  else {
    m_var_names.push_back(varname);
    m_var_sources.push_back(source);
    m_var_first.push_back(timestamp);
    m_var_last.push_back(timestamp);
    m_var_lines.push_back(1);
    m_var_chars.push_back(chars);
    m_vmap[varname] = m_var_names.size()-1;
  }

  m_lines++;
}


//--------------------------------------------------------
// Procedure: fillAppStats()

void ScanReport::fillAppStats()
{
  double total_lines = 0;
  double total_chars = 0;


  // Pass 2A

  int vsize = m_var_names.size();
  for(int i=0; i<vsize; i++) {
    string sources = m_var_sources[i];
    vector<string> ivector = parseString(sources, ',');
    int isize = ivector.size();
    for(int j=0; j<isize; j++) {
      string app_name = ivector[j];
      m_app_lines[app_name] += m_var_lines[i];
      m_app_chars[app_name] += m_var_chars[i];
      m_app_vars[app_name]++;
    }

    total_lines += m_var_lines[i];
    total_chars += m_var_chars[i];
  }

  // Pass 2B - fill in lines, chars as percentage of total
  
  map<string, double>::iterator p;
  p = m_app_lines.begin();
  while(p != m_app_lines.end()) {
    string app_name  = p->first;
    double app_lines = p->second;
    double percent   = app_lines / total_lines;
    m_app_lines_pct[app_name] = percent;
    m_all_sources.push_back(app_name);
    p++;
  }

  p = m_app_chars.begin();
  while(p != m_app_chars.end()) {
    string app_name  = p->first;
    double app_chars = p->second;
    double percent   = app_chars / total_chars;
    m_app_chars_pct[app_name] = percent;
    p++;
  }


}


//--------------------------------------------------------
// Procedure: getVarName

string ScanReport::getVarName(unsigned int index)
{
  if((index >= 0) && (index < m_var_names.size()))
    return(m_var_names[index]);
  else
    return("");
}


//--------------------------------------------------------
// Procedure: getVarSources

string ScanReport::getVarSources(unsigned int index)
{
  if((index >= 0) && (index < m_var_sources.size()))
    return(m_var_sources[index]);
  else
    return("");
}


//--------------------------------------------------------
// Procedure: getVarFirstTime

double ScanReport::getVarFirstTime(unsigned int index)
{
  if((index >= 0) && (index < m_var_first.size()))
    return(m_var_first[index]);
  else
    return(0);
}


//--------------------------------------------------------
// Procedure: getVarLastTime

double ScanReport::getVarLastTime(unsigned int index)
{
  if((index >= 0) && (index < m_var_last.size()))
    return(m_var_last[index]);
  else
    return(0);
}


//--------------------------------------------------------
// Procedure: getVarCount

unsigned int ScanReport::getVarCount(unsigned int index)
{
  if((index >= 0) && (index < m_var_lines.size()))
    return(m_var_lines[index]);
  else
    return(0);
}

//--------------------------------------------------------
// Procedure: getVarChars

unsigned int ScanReport::getVarChars(unsigned int index)
{
  if((index >= 0) && (index < m_var_chars.size()))
    return(m_var_chars[index]);
  else
    return(0);
}


//--------------------------------------------------------
// Procedure: containsVar
//     Notes: 

bool ScanReport::containsVar(const string& varname)
{
  map<string,int>::iterator p;
  p = m_vmap.find(varname);
  if(p != m_vmap.end())
    return(true);
  else
    return(false);
}


//--------------------------------------------------------
// Procedure: getVarIndex
//     Notes: 

int ScanReport::getVarIndex(const string& varname)
{
  map<string,int>::iterator p;
  p = m_vmap.find(varname);
  if(p != m_vmap.end())
    return(p->second);
  else
    return(-1);
}


//--------------------------------------------------------
// Procedure: getMaxLines
//     Notes: 

unsigned int ScanReport::getMaxLines()
{
  unsigned int result = 0;
  unsigned int vsize = m_var_lines.size();
  for(unsigned int i=0; i<vsize; i++) {
    if(m_var_lines[i] > result)
      result = m_var_lines[i];
  }
  return(result);
}


//--------------------------------------------------------
// Procedure: getMaxChars
//     Notes: 

unsigned int ScanReport::getMaxChars()
{
  unsigned int result = 0;
  unsigned int vsize = m_var_chars.size();
  for(unsigned int i=0; i<vsize; i++) {
    if(m_var_chars[i] > result)
      result = m_var_chars[i];
  }
  return(result);
}


//--------------------------------------------------------
// Procedure: getVarNameMaxLength
//     Notes: 

unsigned int ScanReport::getVarNameMaxLength()
{
  unsigned int result = 0;
  unsigned int vsize = m_var_names.size();
  for(unsigned int i=0; i<vsize; i++) {
    if(m_var_names[i].length() > result)
      result = m_var_names[i].length();
  }
  return(result);
}


//--------------------------------------------------------
// Procedure: getMinStartTime
//     Notes: 

double ScanReport::getMinStartTime()
{
  unsigned int vsize = m_var_first.size();
  if(vsize == 0)
    return(0);
  
  double result = m_var_first[0];
  for(unsigned int i=1; i<vsize; i++) {
    if(m_var_first[i] < result)
      if(m_var_first[i] >= 0)
	result = m_var_first[i];
  }
  return(result);
}

//--------------------------------------------------------
// Procedure: getMaxStartTime
//     Notes: 

double ScanReport::getMaxStartTime()
{
  unsigned int vsize = m_var_first.size();
  
  double result = 0;
  for(unsigned int i=0; i<vsize; i++) {
    if(m_var_first[i] > result)
      result = m_var_first[i];
  }
  return(result);
}


//--------------------------------------------------------
// Procedure: getMaxStopTime
//     Notes: 

double ScanReport::getMaxStopTime()
{
  unsigned int vsize = m_var_first.size();
  double result = 0;
  for(unsigned int i=0; i<vsize; i++) {
    if(m_var_last[i] > result)
      result = m_var_last[i];
  }
  return(result);
}


//--------------------------------------------------------
// Procedure: sort
//     Notes: 

void ScanReport::sort(const string& style)
{
  if(style == "bychars_ascending")
    sortByChars(true);
  else if(style == "bychars_descending")
    sortByChars(false);
  else if(style == "bylines_ascending")
    sortByLines(true);
  else if(style == "bylines_descending")
    sortByLines(false);
  else if(style == "bystarttime_ascending")
    sortByStartTime(true);
  else if(style == "bystarttime_descending")
    sortByStartTime(false);
  else if(style == "bystoptime_ascending")
    sortByStopTime(true);
  else if(style == "bystoptime_descending")
    sortByStopTime(false);
}



//--------------------------------------------------------
// Procedure: sortByVarName
//     Notes: 

#if 0
void ScanReport::sortByVarName()
{
  bool done = false;
  while(!done) {
    unsigned int i, vsize = m_var_names.size();
    for(i=0; i<vsize; i++) {
    }
  }
}
#endif


//--------------------------------------------------------
// Procedure: sortByChars
//     Notes: 

void ScanReport::sortByChars(bool ascending)
{
  bool done = false;
  while(!done) {
    done = true;
    unsigned int i, vsize = m_var_names.size();
    for(i=0; i<vsize-1; i++) {
      bool order_switch = false;
      if(ascending  && (m_var_chars[i] > m_var_chars[i+1]))
	order_switch = true;
      if(!ascending && (m_var_chars[i] < m_var_chars[i+1]))
	order_switch = true;
      if(order_switch) {
	done = false;
	switchItems(i, i+1);
      }
    }
  }
}


//--------------------------------------------------------
// Procedure: sortByLines
//     Notes: 

void ScanReport::sortByLines(bool ascending)
{
  bool done = false;
  while(!done) {
    done = true;
    unsigned int i, vsize = m_var_names.size();
    for(i=0; i<vsize-1; i++) {
      bool order_switch = false;
      if(ascending  && (m_var_lines[i] > m_var_lines[i+1]))
	order_switch = true;
      if(!ascending && (m_var_lines[i] < m_var_lines[i+1]))
	order_switch = true;
      if(order_switch) {
	done = false;
	switchItems(i, i+1);
      }
    }
  }
}

//--------------------------------------------------------
// Procedure: sortStartTime
//     Notes: 

void ScanReport::sortByStartTime(bool ascending)
{
  bool done = false;
  while(!done) {
    done = true;
    unsigned int i, vsize = m_var_names.size();
    for(i=0; i<vsize-1; i++) {
      bool order_switch = false;
      if(ascending  && (m_var_first[i] > m_var_first[i+1]))
	order_switch = true;
      if(!ascending && (m_var_first[i] < m_var_first[i+1]))
	order_switch = true;
      if(order_switch) {
	done = false;
	switchItems(i, i+1);
      }
    }
  }
}

//--------------------------------------------------------
// Procedure: sortByStopTime
//     Notes: 

void ScanReport::sortByStopTime(bool ascending)
{
  bool done = false;
  while(!done) {
    done = true;
    unsigned int i, vsize = m_var_names.size();
    for(i=0; i<vsize-1; i++) {
      bool order_switch = false;
      if(ascending  && (m_var_last[i] > m_var_last[i+1]))
	order_switch = true;
      if(!ascending && (m_var_last[i] < m_var_last[i+1]))
	order_switch = true;
      if(order_switch) {
	done = false;
	switchItems(i, i+1);
      }
    }
  }
}


//--------------------------------------------------------
// Procedure: switchItems
//     Notes: 

void ScanReport::switchItems(unsigned int i, unsigned int j)
{
  if(i==j)
    return;

  unsigned int vsize = m_var_names.size();
  if((i >= vsize) || (j >= vsize))
    return;

  m_vmap[m_var_names[i]] = j;
  m_vmap[m_var_names[j]] = i;
  unsigned int tmp_int;
  string       tmp_str;
  double       tmp_dbl;
  tmp_str        = m_var_names[i];
  m_var_names[i] = m_var_names[j];
  m_var_names[j] = tmp_str;
  
  tmp_str          = m_var_sources[i];
  m_var_sources[i] = m_var_sources[j];
  m_var_sources[j] = tmp_str;
  
  tmp_dbl        = m_var_first[i];
  m_var_first[i] = m_var_first[j];
  m_var_first[j] = tmp_dbl;
  
  tmp_dbl        = m_var_last[i];
  m_var_last[i]  = m_var_last[j];
  m_var_last[j]  = tmp_dbl;
  
  tmp_int        = m_var_lines[i];
  m_var_lines[i] = m_var_lines[j];
  m_var_lines[j] = tmp_int;
  
  tmp_int        = m_var_chars[i];
  m_var_chars[i] = m_var_chars[j];
  m_var_chars[j] = tmp_int;
}

