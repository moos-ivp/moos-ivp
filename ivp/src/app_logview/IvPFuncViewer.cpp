/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPFuncViewer.cpp                                    */
/*    DATE: Feb 25th, 2007                                       */
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "IvPFuncViewer.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"

using namespace std;

IvPFuncViewer::IvPFuncViewer(int x, int y, int w, int h, const char *l)
  : Common_IPFViewer(x,y,w,h,l)
{
  m_plot_ix    = 0;
  m_rad_extra  = 20;
  m_draw_frame = false;
  m_zoom       = 2.0;
  m_collective = false;

  setParam("reset_view", "2");

  m_clear_color.setColor("0.6,0.8,0.4");
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: draw()

void IvPFuncViewer::draw()
{
  Common_IPFViewer::draw();
  if(m_draw_frame)
    drawFrame();
}

//-------------------------------------------------------------
// Procedure: addIPF_Plot
//      Note: A local copy of the given IPF_Plot is created here.

unsigned int IvPFuncViewer::addIPF_Plot(const IPF_Plot& g_ipf_plot, 
					bool make_this_active)
{
  string ipf_source = g_ipf_plot.getSource();
  string ipf_vname  = g_ipf_plot.getVName();

  m_ipf_vname.push_back(ipf_vname);
  m_ipf_source.push_back(ipf_source);
  m_ipf_plot.push_back(g_ipf_plot);

  if(make_this_active)
    m_plot_ix = m_ipf_plot.size()-1;

  return(m_ipf_plot.size());
}

//-------------------------------------------------------------
// Procedure: altPlotIndex
//            Ensure that the new index (m_plot_ix) is >= 0 since
//            it is an unsigned int.

void IvPFuncViewer::altPlotIndex(int val)
{
  if((val > 0) && ((m_plot_ix + val) < m_ipf_plot.size()))
    m_plot_ix += val;
  else if((val < 0) && (m_plot_ix >= -val))
    m_plot_ix -= val;
  else
    return;

  setVNameIPF(m_ipf_vname[m_plot_ix]);
  setSourceIPF(m_ipf_source[m_plot_ix]);
}

//-------------------------------------------------------------
// Procedure: setPlotIndex

void IvPFuncViewer::setPlotIndex(unsigned int new_index)
{
  if(new_index < m_ipf_plot.size())
    m_plot_ix = new_index;
  else
    return;
  m_collective = false;

  setVNameIPF(m_ipf_vname[m_plot_ix]);
  setSourceIPF(m_ipf_source[m_plot_ix]);
}

//-------------------------------------------------------------
// Procedure: setCurrTime

void IvPFuncViewer::setCurrTime(double curr_time)
{
  if(m_plot_ix >= m_ipf_plot.size())
    return;
  
  if(curr_time > m_ipf_plot[m_plot_ix].getMaxTime())
    curr_time = m_ipf_plot[m_plot_ix].getMaxTime();
  if(curr_time < m_ipf_plot[m_plot_ix].getMinTime())
    curr_time = m_ipf_plot[m_plot_ix].getMinTime();  

  if(m_collective)
    buildCollective(curr_time);
  else {
    unsigned int iter = m_viter_map[m_ipf_vname[m_plot_ix]];
    string ipf_string = m_ipf_plot[m_plot_ix].getIPFByHelmIteration(iter);
    setVNameIPF(m_ipf_vname[m_plot_ix]);
    setSourceIPF(m_ipf_source[m_plot_ix]);
    setIterIPF(intToString(iter));
    applyIPF(ipf_string);
  }
}

//-------------------------------------------------------------
// Procedure: buildCollective

void IvPFuncViewer::buildCollective(double curr_time)
{
  // Phase 1: Determine the current helm iteration
  unsigned int curr_iter = m_ipf_plot[m_plot_ix].getHelmIterByTime(curr_time);

  // Phase 2: Get all the IvPFunction strings for the current iteration
  // for the current vehicle.
  string curr_vname = m_ipf_vname[m_plot_ix];

  vector<string> ipfs;
  unsigned int i, vsize = m_ipf_plot.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf_vname[i] == curr_vname) {
      string ipf_str = m_ipf_plot[i].getIPFByHelmIteration(curr_iter);
      if(ipf_str != "")
	ipfs.push_back(ipf_str);
    }
  }
  // Phase 3: Build the collective of the given functions.
  m_quadset.clear();
  for(i=0; i<ipfs.size(); i++) {
    QuadSet quadset = setQuadSetFromIPF(ipfs[i]);
    bool ok = m_quadset.addQuadSet(&quadset);
  }

  m_quadset.applyColorMap(m_color_map);
  m_quadset.normalize(0, 200);
}
