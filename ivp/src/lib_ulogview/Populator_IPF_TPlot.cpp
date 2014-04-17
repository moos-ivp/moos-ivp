/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_IPF_TPlot.cpp                              */
/*    DATE: Feb2407, Sep2811                                     */
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

#include <iostream>
#include <cstdlib>
#include "Populator_IPF_TPlot.h"
#include "MBUtils.h"
#include "IvPFunction.h"
#include "FunctionEncoder.h"
#include "FileBuffer.h"
#include "DemuxedResult.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populate

bool Populator_IPF_TPlot::populateFromEntries(const vector<ALogEntry>& entries)
{
  unsigned int i, vsize = entries.size();  
  for(i=0; i<vsize; i++) {
    if(entries[i].getVarName() == "BHV_IPF") {
      double time_stamp = entries[i].getTimeStamp();
      string var_value  = entries[i].getStringVal();
      m_demuxer.addMuxPacket(var_value, time_stamp);
    }
  }
  
  bool done = false;
  while(!done) {
    DemuxedResult result = m_demuxer.getDemuxedResult();
    
    double time_stamp   = result.getTStamp();
    string demux_string = result.getString();
    if(demux_string != "")
      handleEntry(time_stamp, demux_string);
    else
      done = true;
  }
  return(true);
}


//---------------------------------------------------------------
// Procedure: setLogStartUTC

void Populator_IPF_TPlot::setLogStartUTC(double utc_time)
{
  m_logstart_utc =  utc_time;
  m_iter_plot.setLogStartUTC(utc_time);
  m_null_plot.setLogStartUTC(utc_time);
}

//---------------------------------------------------------------
// Procedure: getPlotIPF

const IPF_TPlot& Populator_IPF_TPlot::getPlotIPF(unsigned int ix)
{
  if(ix >= m_ipf_plots.size())
    return(m_null_plot);
  else
    return(m_ipf_plots[ix]);
}

//---------------------------------------------------------------
// Procedure: getTagIPF

string Populator_IPF_TPlot::getTagIPF(unsigned int ix)
{
  if(ix >= m_ipf_tags.size())
    return("");
  else
    return(m_ipf_tags[ix]);
  
}

//---------------------------------------------------------------
// Procedure: print

void Populator_IPF_TPlot::print()
{
  int tag_size = m_ipf_tags.size();
  int ipf_size = m_ipf_plots.size();

  cout << "Number of IPF_TPlots: " << ipf_size << endl;
  cout << "Number of Tags: " << tag_size << endl;

  if(tag_size != ipf_size) {
    cout << "FIGURE OUT WHY THESE ARE DIFFERENT!!!!" << endl;
    return;
  }

  for(int i=0; i<tag_size; i++) {
    cout << "Item[" << i << "]: " << endl;
    cout << "  Tag: " << m_ipf_tags[i] << endl;
    cout << "  Plot[" << i << "]: " << endl;
    cout << "    Entries: " << m_ipf_plots[i].size() << endl;
    cout << "    LowStamp: " << m_ipf_plots[i].getMinTime() << endl;
    cout << "    HighStamp: " << m_ipf_plots[i].getMaxTime() << endl;
    cout << "    IPF_Source: " << m_ipf_plots[i].getSource() << endl;
    cout << "    IPF_VName: "  << m_ipf_plots[i].getVName() << endl;
  }
}


//---------------------------------------------------------------
// Procedure: handleEntry

void Populator_IPF_TPlot::handleEntry(double g_time, 
				     const string& g_ipf_str)
{
  IvPFunction *ipf = StringToIvPFunction(g_ipf_str);
  if(!ipf) {
    cout << "Unable to create IvPFunction from string" << endl;
    return;
  }

  string    context    = ipf->getContextStr();
  int       ipf_pieces = ipf->size();
  string    ipf_source;
  int       ipf_iteration = 0;
  double    ipf_pwt    = ipf->getPWT();
  IvPDomain ivp_domain = ipf->getPDMap()->getDomain();

  // The Context string might come in the form of NUM:CONTEXT"
  vector<string> svector = parseString(context, ':');
  if(svector.size() == 2) {
    ipf_iteration = atoi(svector[0].c_str());
    ipf_source    = svector[1];
  }
  string tag = (m_vname + "_" + ipf_source);

  delete(ipf);

  bool found = false;
  unsigned int index = 0;
  unsigned int i, vsize = m_ipf_tags.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf_tags[i] == tag) {
      index = i;
      found = true;
    }
  }
  
  if(!found) {
    IPF_TPlot new_plot;
    new_plot.setIvPDomain(m_ivp_domain);
    new_plot.setLogStartUTC(m_logstart_utc);
    new_plot.setSource(ipf_source);
    new_plot.setVName(m_vname);
    m_ipf_tags.push_back(tag);
    m_ipf_plots.push_back(new_plot);
    index = vsize;
  }
  
  m_iter_plot.setValue(g_time, ipf_iteration);
  m_ipf_plots[index].addEntry(g_time, g_ipf_str, ipf_iteration, ipf_pieces,
			      ipf_pwt, ivp_domain);
}





