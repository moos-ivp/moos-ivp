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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "IvPFuncViewer.h"
#include "FunctionEncoder.h"
#include "IPFViewUtils.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

IvPFuncViewer::IvPFuncViewer(int x, int y, int w, int h, const char *l)
  : Common_IPFViewer(x,y,w,h,l)
{
  m_plot_ix       = 0;
  m_rad_extra     = 15;
  m_draw_frame    = true;
  m_zoom          = 2.0;
  m_collective_ix = -1;
  m_mouse_infocus = false;

  m_collective_by_default = false;

  setParam("reset_view", "2");

  m_clear_color.setColor("0.6,0.7,0.5");
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: handle()

int IvPFuncViewer::handle(int event)
{
  if(event == 4)
    m_mouse_infocus = true;
  else if(event == 3)
    m_mouse_infocus = false;
  else if((event == 12) && m_mouse_infocus)
    return(0);
  else
    return(Fl_Gl_Window::handle(event));
  return(0);
}


//-------------------------------------------------------------
// Procedure: draw()

void IvPFuncViewer::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  bool result = Common_IPFViewer::drawIvPFunction();
  if(result) {
    //if(m_draw_frame)
    //  drawFrame();
    drawOwnPoint();

    unsigned int max_crs_qix = m_quadset.getMaxPointQIX("course");
    unsigned int max_spd_qix = m_quadset.getMaxPointQIX("speed");
    
    if(m_draw_pin)
      Common_IPFViewer::drawMaxPoint(max_crs_qix, max_spd_qix);
  }

  glPopMatrix();
  glFlush();
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

  // If collective_by_default set this viewer to display the 
  // collective objective function as soon as it becomes non-empty.
  if(m_collective_by_default)
    m_collective_ix = 0;

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
    double pwt = m_ipf_plot[m_plot_ix].getPwtByHelmIteration(iter);
    setVNameIPF(m_ipf_vname[m_plot_ix]);
    setSourceIPF(m_ipf_source[m_plot_ix]);
    setIterIPF(intToString(iter));
    setPiecesIPF(intToString(pcs));
    setPriorityIPF(dstringCompact(doubleToString(pwt)));
    IvPDomain ivp_domain = m_ipf_plot[m_plot_ix].getIvPDomain();
    IvPDomain ivp_subdomain = m_ipf_plot[m_plot_ix].getDomainByHelmIteration(iter);
    setDomainIPF(ivp_domain);
    setSubDomainIPF(ivp_subdomain);
    
    if(ipf_string == "") 
      m_quadset = QuadSet();
    else {
      IvPFunction *ipf = StringToIvPFunction(ipf_string);
      if(ipf) {
	ipf = expandHdgSpdIPF(ipf, ivp_domain);
	bool ok = m_quadset.applyIPF(ipf);
	if(!ok)
	  m_quadset = QuadSet();
	delete(ipf);
	m_quadset.normalize(0, 100);
	m_quadset.applyColorMap(m_color_map);	
      }
    }
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
  // for the current vehicle, along with the IvPDomain for each ipf.
  vector<string> ipfs;
  vector<IvPDomain> ivp_domains;

  unsigned int i, vsize = m_ipf_plot.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf_vname[i] == curr_vname) {
      string ipf_str = m_ipf_plot[i].getIPFByHelmIteration(curr_iter);
      if(ipf_str != "") {
	IvPDomain ivp_domain = m_ipf_plot[i].getIvPDomain();
	ipfs.push_back(ipf_str);
	ivp_domains.push_back(ivp_domain);
      }
    }
  }

  // Phase 4: Build the collective of the given functions.
  m_quadset = QuadSet();
  
  for(i=0; i<ipfs.size(); i++) {
    // We grab the IvPDomain associated with each IPF string and pass
    // it along to the setQuadSetFromIPF function. This domain should
    // be for the domain of the helm the produced this IPF, not 
    // necessarily the domain of given IPF, which may be a subdomain.
    IvPDomain ivp_domain = ivp_domains[i];

    QuadSet      quadset;
    IvPFunction *ipf = StringToIvPFunction(ipfs[i]);
    ipf = expandHdgSpdIPF(ipf, ivp_domain);
    if(ipf) {
      quadset.applyIPF(ipf);
      delete(ipf);
    }

    if(quadset.size() != 0)
      m_quadset.addQuadSet(quadset);
  }
  m_quadset.normalize(0, 100);
  m_quadset.applyColorMap(m_color_map);

  // Phase 5: Set the text information for display
  setVNameIPF(curr_vname);
  setIterIPF(intToString(curr_iter));
  setSourceIPF("collective");
  setPiecesIPF("");
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
  

//-------------------------------------------------------------
// Procedure: getCurrVName()

string IvPFuncViewer::getCurrVName() const
{
  if(m_collective_ix >= 0) {
    if((unsigned int)(m_collective_ix) < m_all_vnames.size())
      return(m_all_vnames[m_collective_ix]);
    else
      return("n/a");
  }

  if(m_ipf_vname.size() > 0)
    return(m_ipf_vname[m_plot_ix]);
  else
    return("n/a");
}
  
//-------------------------------------------------------------
// Procedure: getCurrSource()

string IvPFuncViewer::getCurrSource() const
{
  if(m_collective_ix >= 0) {
    if((unsigned int)(m_collective_ix) < m_all_vnames.size())
      return("Collective");
    else
      return("n/a");
  }

  if(m_ipf_source.size() > 0)
    return(m_ipf_source[m_plot_ix]);
  else
    return("n/a");
}
  
//-------------------------------------------------------------
// Procedure: getCurrPieces()

string IvPFuncViewer::getCurrPieces() const
{
  return(m_active_ipf_pieces);
}
  
//-------------------------------------------------------------
// Procedure: getCurrPriority()

string IvPFuncViewer::getCurrPriority() const
{
  return(m_active_ipf_priority);
}
  
//-------------------------------------------------------------
// Procedure: getCurrDomain()

string IvPFuncViewer::getCurrDomain() const
{
  string domain_str = domainToString(m_active_ipf_subdomain, false);
  return(domain_str);
}
  
//-------------------------------------------------------------
// Procedure: getCurrIteration()

string IvPFuncViewer::getCurrIteration() const
{
  return(m_active_ipf_iter);
}
  
