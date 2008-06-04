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
    m_var_count[index]++;
    m_var_chars[index] += chars;
  }
  else {
    m_var_names.push_back(varname);
    m_var_sources.push_back(source);
    m_var_first.push_back(timestamp);
    m_var_last.push_back(timestamp);
    m_var_count.push_back(1);
    m_var_chars.push_back(chars);
    m_vmap[varname] = m_var_names.size()-1;
  }

  m_lines++;
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
  if((index >= 0) && (index < m_var_count.size()))
    return(m_var_count[index]);
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
  unsigned int vsize = m_var_count.size();
  for(unsigned int i=0; i<vsize; i++) {
    if(m_var_count[i] > result)
      result = m_var_count[i];
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


