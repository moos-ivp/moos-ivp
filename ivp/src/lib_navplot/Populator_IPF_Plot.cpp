/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_IPF_Plot.cpp                               */
/*    DATE: Feb 24th, 2007                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "Populator_IPF_Plot.h"
#include "MBUtils.h"
#include "IvPFunction.h"
#include "FunctionEncoder.h"
#include "FileBuffer.h"
#include "DemuxedResult.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populate

bool Populator_IPF_Plot::populateFromEntries(const vector<ALogEntry>& entries)
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
// Procedure: handleEntry

void Populator_IPF_Plot::handleEntry(double g_time, 
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
  int       ipf_iteration;
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

  
  int index = -1;
  int vsize = m_ipf_tags.size();
  for(int i=0; i<vsize; i++) {
    if(m_ipf_tags[i] == tag)
      index = i;
  }

  if(index == -1) {
    IPF_Plot new_plot;
    new_plot.setSource(ipf_source);
    new_plot.setVName(m_vname);
    m_ipf_tags.push_back(tag);
    m_ipf_plots.push_back(new_plot);
    index = vsize;
  }
  
  m_ipf_plots[index].addEntry(g_time, g_ipf_str, ipf_iteration, ipf_pieces,
			      ipf_pwt, ivp_domain);
}

//---------------------------------------------------------------
// Procedure: getPlotIPF

IPF_Plot Populator_IPF_Plot::getPlotIPF(unsigned int ix)
{
  if(ix >= m_ipf_plots.size()) {
    IPF_Plot null_plot;
    return(null_plot);
  }
  else
    return(m_ipf_plots[ix]);
}

//---------------------------------------------------------------
// Procedure: getTagIPF

string Populator_IPF_Plot::getTagIPF(unsigned int ix)
{
  if(ix >= m_ipf_tags.size())
    return("");
  else
    return(m_ipf_tags[ix]);
  
}

//---------------------------------------------------------------
// Procedure: print

void Populator_IPF_Plot::print()
{
  int tag_size = m_ipf_tags.size();
  int ipf_size = m_ipf_plots.size();

  cout << "Number of IPF_Plots: " << ipf_size << endl;
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







