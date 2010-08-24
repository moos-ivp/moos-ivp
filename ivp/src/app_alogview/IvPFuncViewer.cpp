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
  m_plot_ix       = 0;
  m_rad_extra     = 20;
  m_draw_frame    = false;
  m_zoom          = 2.0;
  m_collective_ix = -1;

  setParam("reset_view", "2");

  m_clear_color.setColor("0.6,0.7,0.5");
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: draw()

void IvPFuncViewer::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  Common_IPFViewer::drawIvPFunction();
  if(m_draw_frame)
    drawFrame();
  drawOwnPoint();
  glPopMatrix();
  
  glFlush();

  //GLenum err = glGetError();
  //if(err != GL_NO_ERROR)
  //  cout << "WARNING!!!!! GL ERROR DETECTED!!!!" << endl;
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

  // Add the vehicle name to the vector of all vehicle names if
  // does not already exist in the vector.
  int ix = getVNameIndex(ipf_vname);
  if(ix == -1)
    m_all_vnames.push_back(ipf_vname);

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
  else if((val < 0) && ((m_plot_ix-val) >= 0))
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

  m_collective_ix = -1;

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

  if(m_collective_ix != -1)
    buildCollective(curr_time);
  else {
    unsigned int iter = m_viter_map[m_ipf_vname[m_plot_ix]];
    string ipf_string = m_ipf_plot[m_plot_ix].getIPFByHelmIteration(iter);
    unsigned int pcs = m_ipf_plot[m_plot_ix].getPcsByHelmIteration(iter);
    setVNameIPF(m_ipf_vname[m_plot_ix]);
    setSourceIPF(m_ipf_source[m_plot_ix]);
    setIterIPF(intToString(iter));
    setPiecesIPF(intToString(pcs));
    applyIPF(ipf_string);
  }
}

//-------------------------------------------------------------
// Procedure: setCollectiveIndex(index)
//      Note: An index of -1 indicates that the collective function
//            is not to be drawn. 

void IvPFuncViewer::setCollectiveIndex(int index)
{
  if((index >= 0) && ((unsigned int)(index) < m_all_vnames.size()))
    m_collective_ix = (unsigned int)(index);
  else
    m_collective_ix = -1;
}


//-------------------------------------------------------------
// Procedure: buildCollective

void IvPFuncViewer::buildCollective(double curr_time)
{
  if(m_collective_ix < 0)
    return;

  if((unsigned int)(m_collective_ix) >= m_all_vnames.size())
    return;

  // Phase 1: Determine the vehicle name for the collective function
  string curr_vname = m_all_vnames[m_collective_ix];

  // Phase 2: Determine the current helm iteration
  unsigned int curr_iter = m_viter_map[curr_vname];

  // Phase 3: Get all the IvPFunction strings for the current iteration
  // for the current vehicle.
  vector<string> ipfs;
  unsigned int i, vsize = m_ipf_plot.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf_vname[i] == curr_vname) {
      string ipf_str = m_ipf_plot[i].getIPFByHelmIteration(curr_iter);
      if(ipf_str != "")
	ipfs.push_back(ipf_str);
    }
  }

  // Phase 4: Build the collective of the given functions.
  m_quadset.clear();
  for(i=0; i<ipfs.size(); i++) {
    QuadSet quadset = setQuadSetFromIPF(ipfs[i]);
    bool ok = m_quadset.addQuadSet(&quadset);
    if(!ok) {
      m_quadset.clear();
      cout << "Error creating collective quadset" << endl;
      return;
    }
  }

  // Phase 5: Set the text information for display
  setVNameIPF(curr_vname);
  setIterIPF(intToString(curr_iter));
  setSourceIPF("collective");
  setPiecesIPF("");

  m_quadset.applyColorMap(m_color_map);
  m_quadset.normalize(0, 200);
}


//-------------------------------------------------------------
// Procedure: getVNameIndex
//      Note: Returns -1 if vehicle name is not found

int IvPFuncViewer::getVNameIndex(string vname)
{
  unsigned int i, vsize = m_all_vnames.size();
  for(i=0; i<vsize; i++) {
    if(m_all_vnames[i] == vname)
      return((int)(i));
  }  
  return(-1);
}
  
