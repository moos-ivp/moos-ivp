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

  m_rad_extra = 20;

  setParam("reset_view", "2");
  m_clear_red   =  73.0 / 256.0;
  m_clear_green =  62.0 / 256.0;
  m_clear_blue  = 120.0 / 256.0;

  m_clear_red   = 0.6;
  m_clear_green = 0.8;
  m_clear_blue  = 0.4;

  m_draw_frame = false;
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: draw()

void IvPFuncViewer::draw()
{
  Common_IPFViewer::draw();
  if(m_draw_frame)
    drawFrame();

  unsigned int i, vsize = m_ipf_source.size();
  for(i=0; i<vsize; i++)
    cout << "src[" << i << "]: " << m_ipf_source[i] << endl;
}

//-------------------------------------------------------------
// Procedure: addIPF_Plot
//      Note: A local copy of the given IPF_Plot is created here.

unsigned int IvPFuncViewer::addIPF_Plot(const IPF_Plot& g_ipf_plot)
{
  string ipf_source = g_ipf_plot.getSource();
  string ipf_vname  = g_ipf_plot.getVName();

  m_ipf_vname.push_back(ipf_vname);
  m_ipf_source.push_back(ipf_source);
  m_ipf_plot.push_back(g_ipf_plot);

  return(m_ipf_plot.size());
}


//-------------------------------------------------------------
// Procedure: setCurrTime

void IvPFuncViewer::setCurrTime(double g_time)
{
  if(m_plot_ix >= m_ipf_plot.size())
    return;

  string ipf_string = m_ipf_plot[m_plot_ix].getIPFByTime(g_time);

  IvPFunction *ipf = StringToIvPFunction(ipf_string);

  setVNameIPF(m_ipf_vname[m_plot_ix]);
  setSourceIPF(m_ipf_source[m_plot_ix]);
  applyIPF(ipf, true);
}

//-------------------------------------------------------------
// Procedure: altPlotIndex

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

void IvPFuncViewer::setPlotIndex(unsigned int val)
{
  if(val < m_ipf_plot.size())
    m_plot_ix = val;
  else
    return;

  setVNameIPF(m_ipf_vname[m_plot_ix]);
  setSourceIPF(m_ipf_source[m_plot_ix]);
}
