/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: VarPlot.cpp                                          */
/*    DATE: Oct 10th 2011                                        */
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

#include <list>
#include <iostream>
#include "VarPlot.h"
#include "MBUtils.h"
#include "LogUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

VarPlot::VarPlot()
{
  m_base_utc = 0;

  m_max_len_source = 0;
  m_max_len_srcaux = 0;
}

//---------------------------------------------------------------
// Procedure: setValue()

bool VarPlot::setValue(double gtime, string gvalue, string gsource)
{
  unsigned int tsize = m_time.size();

  if((tsize == 0) || (m_time[tsize-1] <= gtime)) {
    m_time.push_back(gtime);
    m_entry.push_back(gvalue);

    string source = gsource;
    string srcaux = "";
    if(strContains(gsource, ':')) {
      source = biteStringX(gsource, ':');
      srcaux = gsource;
    }
    m_source.push_back(source);
    m_srcaux.push_back(srcaux);

    if(source.length() > m_max_len_source)
      m_max_len_source = source.length();
    if(srcaux.length() > m_max_len_srcaux)
      m_max_len_srcaux = srcaux.length();
    return(true);
  }
  else
    return(false);
}

//---------------------------------------------------------------
// Procedure: applySkew()

void VarPlot::applySkew(double skew)
{
  for(unsigned int i=0; i<m_time.size(); i++)
    m_time[i] += skew;
}
     

//---------------------------------------------------------------
// Procedure: containsTime()

bool VarPlot::containsTime(double gtime) const
{
  unsigned int vsize = m_time.size();
  if(vsize == 0)
    return(false);

  if((gtime < m_time[0]) || (gtime > m_time[vsize-1]))
    return(false);

  return(true);
}
     

//---------------------------------------------------------------
// Procedure: print()

void VarPlot::print() const
{
  unsigned int i;
  cout << "VarPlot::print()" << endl;
  cout << " Variable Name: " << m_varname << endl;
  for(i=0; i<m_entry.size(); i++) {
    cout << "time:" << m_time[i];
    cout << "  val:" << m_entry[i] << endl;
  }
}


//---------------------------------------------------------------
// Procedure: getEntryByIndex()

string VarPlot::getEntryByIndex(unsigned int index) const
{  
  if(index < m_entry.size())
    return(m_entry[index]);
  return("");
}

//---------------------------------------------------------------
// Procedure: getSourceByIndex()

string VarPlot::getSourceByIndex(unsigned int index) const
{  
  if(m_srcname != "")
    return(m_srcname);

  if(index < m_source.size())
    return(m_source[index]);
  return("");
}

//---------------------------------------------------------------
// Procedure: getTStampByIndex()

double VarPlot::getTStampByIndex(unsigned int index) const
{  
  if(index < m_time.size())
    return(m_time[index]);
  return(0);
}

//---------------------------------------------------------------
// Procedure: getEntryByTime()

string VarPlot::getEntryByTime(double gtime) const
{
  if(!containsTime(gtime))
    return("");

  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_entry[index]);
}
     

//---------------------------------------------------------------
// Procedure: getSourceByTime()

string VarPlot::getSourceByTime(double gtime) const
{
  if(m_srcname != "")
    return(m_srcname);

  if(!containsTime(gtime))
    return("");

  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_source[index]);
}
     
//---------------------------------------------------------------
// Procedure: getTStampByTime()

double VarPlot::getTStampByTime(double gtime) const
{
  if(!containsTime(gtime))
    return(0);

  unsigned int index = getIndexByTime(m_time, gtime);
  return(m_time[index]);
}
     

//---------------------------------------------------------------
// Procedure: getVarPlotEntriesUpToTime()

list<VarPlotEntry> VarPlot::getVarPlotEntriesUpToTime(double gtime) const
{
  list<VarPlotEntry> rlist;
  if((m_time.size() == 0) || (gtime < m_time[0]))
    return(rlist);

  unsigned int index = getIndexByTime(m_time, gtime);
  for(unsigned int i=0; i<=index; i++) {
    VarPlotEntry entry(m_time[i]);
    entry.setVName(m_vname);
    entry.setVarName(m_varname);
    entry.setVarVal(m_entry[i]);
    string source = m_srcname;
    if((source == "") && (i < m_source.size()))
      source = m_source[i];
    entry.setVarSource(source);
    entry.setVarSrcAux(m_srcaux[i]);
    rlist.push_back(entry);
  }

  return(rlist);
}
     
//---------------------------------------------------------------
// Procedure: getVarPlotEntriesPastTime()

list<VarPlotEntry> VarPlot::getVarPlotEntriesPastTime(double gtime) const
{
  list<VarPlotEntry> rlist;
  if(m_time.size() == 0)
    return(rlist);

  unsigned int index = getIndexByTime(m_time, gtime)+1;
  if(gtime < m_time[0])
    index = 0;

  for(unsigned int i=index; i<m_entry.size(); i++) {
    VarPlotEntry entry(m_time[i]);
    entry.setVName(m_vname);
    entry.setVarName(m_varname);
    entry.setVarVal(m_entry[i]);
    string source = m_srcname;
    if((source == "") && (i < m_source.size()))
      source = m_source[i];
    entry.setVarSource(source);
    entry.setVarSrcAux(m_srcaux[i]);
    rlist.push_back(entry);
  }

  return(rlist);
}
     
//---------------------------------------------------------------
// Procedure: getEntriesUpToTime()

vector<string> VarPlot::getEntriesUpToTime(double gtime) const
{
  vector<string> rvector;
  if((m_time.size() == 0) || (gtime < m_time[0]))
    return(rvector);

  unsigned int index = getIndexByTime(m_time, gtime);
  for(unsigned int i=0; i<=index; i++) 
    rvector.push_back(m_entry[i]);

  return(rvector);
}
     
//---------------------------------------------------------------
// Procedure: getSourcesUpToTime()

vector<string> VarPlot::getSourcesUpToTime(double gtime) const
{
  vector<string> rvector;
  if((m_time.size() == 0) || (gtime < m_time[0]))
    return(rvector);

  unsigned int index = getIndexByTime(m_time, gtime);
  for(unsigned int i=0; i<=index; i++) {
    if(m_srcname.length() != 0)
      rvector.push_back(m_srcname);
    else
      rvector.push_back(m_source[i]);
  }
  return(rvector);
}
     
//---------------------------------------------------------------
// Procedure: getTStampsUpToTime()

vector<double> VarPlot::getTStampsUpToTime(double gtime) const
{
  vector<double> rvector;
  if((m_time.size() == 0) || (gtime < m_time[0]))
    return(rvector);

  unsigned int index = getIndexByTime(m_time, gtime);
  for(unsigned int i=0; i<=index; i++)
    rvector.push_back(m_time[i]);

  return(rvector);
}
     
//---------------------------------------------------------------
// Procedure: getEntriesPastTime()

vector<string> VarPlot::getEntriesPastTime(double gtime) const
{
  vector<string> rvector;
  if(m_time.size() == 0)
    return(rvector);

  unsigned int index = getIndexByTime(m_time, gtime)+1;
  if(gtime < m_time[0])
    index = 0;

  for(unsigned int i=index; i<m_time.size(); i++) 
    rvector.push_back(m_entry[i]);

  return(rvector);
}
     
//---------------------------------------------------------------
// Procedure: getSourcesPastTime()

vector<string> VarPlot::getSourcesPastTime(double gtime) const
{
  vector<string> rvector;
  if(m_time.size() == 0)
    return(rvector);

  unsigned int index = getIndexByTime(m_time, gtime)+1;
  if(gtime < m_time[0])
    index = 0;

  for(unsigned int i=index; i<m_time.size(); i++) {
    if(m_srcname.length() != 0)
      rvector.push_back(m_srcname);
    else
      rvector.push_back(m_source[i]);
  }
  return(rvector);
}
     
//---------------------------------------------------------------
// Procedure: getTStampsUpToTime()

vector<double> VarPlot::getTStampsPastTime(double gtime) const
{
  vector<double> rvector;
  if(m_time.size() == 0)
    return(rvector);

  unsigned int index = getIndexByTime(m_time, gtime)+1;
  if(gtime < m_time[0])
    index = 0;

  for(unsigned int i=index; i<m_time.size(); i++) 
    rvector.push_back(m_time[i]);

  return(rvector);
}
     





