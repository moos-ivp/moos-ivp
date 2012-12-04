/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPFuncViewerX.cpp                                   */
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
#include "IvPFuncViewerX.h"
#include "FunctionEncoder.h"
#include "IPFViewUtils.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "MBTimer.h"

using namespace std;

IvPFuncViewerX::IvPFuncViewerX(int x, int y, int w, int h, const char *l)
  : Common_IPFViewer(x,y,w,h,l)
{
  m_rad_extra     = 1;
  m_zoom          = 2.0;

  m_curr_time       = 0;
  m_curr_iter       = 0;
  m_min_time_utc    = 0;
  m_max_time_utc    = 0;

  m_time_format_utc = true;
  
  setParam("reset_view", "2");

  m_clear_color.setColor("0.6,0.7,0.5");
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: draw()

void IvPFuncViewerX::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  bool result = Common_IPFViewer::drawIvPFunction();

  if(result && (m_quadset.getQuadSetDim() == 2)) {
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
// Procedure: handle()

int IvPFuncViewerX::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}

//-------------------------------------------------------------
// Procedure: addIPF_Plot
//      Note: A local copy of the given IPF_TPlot is created here.

void IvPFuncViewerX::addIPF_Plot(const IPF_TPlot& g_ipf_plot)
{
  string ipf_source = g_ipf_plot.getSource();
  string ipf_vname  = g_ipf_plot.getVName();

  if(m_vname == "")
    m_vname = ipf_vname;
  else if(m_vname != ipf_vname)
    return;

  m_map_ipf_plots[ipf_source] = g_ipf_plot;
  
  if(m_source == "")
    m_source = ipf_source;
}

//-------------------------------------------------------------
// Procedure: setMinMaxTimeUTC

void IvPFuncViewerX::setMinMaxTimeUTC(double mintime, double maxtime)
{
  if(mintime > maxtime)
    return;
  
  m_min_time_utc = mintime;
  m_max_time_utc = maxtime;
}

//-------------------------------------------------------------
// Procedure: setTimeFormat

void IvPFuncViewerX::setTimeFormat(const string& format)
{
  if(format == "utc")
    m_time_format_utc = true;
  else if(format == "local")
    m_time_format_utc = false;
  else if(format == "toggle")
    m_time_format_utc = !m_time_format_utc;
}


//-------------------------------------------------------------
// Procedure: incrementIter

void IvPFuncViewerX::incrementIter(int amt)
{
  unsigned int new_iter = m_curr_iter;
  if((amt > 0) && ((m_curr_iter + amt) <= m_iter_plot.getMaxIter()))
    new_iter += (unsigned int)(amt);
  if((amt < 0) && ((m_curr_iter - amt) >=  m_iter_plot.getMinIter()))
    new_iter += (unsigned int)(amt);

  double prev_time = m_curr_time;
  setTimeUTCFromIteration(new_iter);

  if(m_curr_time != prev_time)
    m_pbuffer.pushPending("ALOGVIEW_TIME", m_curr_time);

  updateIPF();
}



//-------------------------------------------------------------
// Procedure: setTime()
//      Note: If the time is specified by helm iteration, it takes precedent 

void IvPFuncViewerX::setTime(const ULV_TimeStamp& timestamp)
{
  if(timestamp.vname() == m_vname)
    setTimeUTCFromIteration(timestamp.iter());
  else
    setIterationFromTimeUTC(timestamp.time());
  
  updateIPF();
}

//-------------------------------------------------------------
// Procedure: setTime()

void IvPFuncViewerX::setTime(double time_utc)
{
  setIterationFromTimeUTC(time_utc);
  updateIPF();
}


//-------------------------------------------------------------
// Procedure: setSource

void IvPFuncViewerX::setSource(string source)
{
  if(m_map_ipf_plots.count(source) || strBegins(source, "collective"))
    m_source = source;
  updateIPF();
}


//-------------------------------------------------------------
// Procedure: getCurrPieces()

string IvPFuncViewerX::getCurrPieces() const
{
  return(m_active_ipf_pieces);
}
  
//-------------------------------------------------------------
// Procedure: getCurrPriority()

string IvPFuncViewerX::getCurrPriority() const
{
  return(m_active_ipf_priority);
}
  
//-------------------------------------------------------------
// Procedure: getCurrDomain()

string IvPFuncViewerX::getCurrDomain() const
{
  string domain_str = domainToString(m_active_ipf_subdomain, false);
  return(domain_str);
}
  
//-------------------------------------------------------------
// Procedure: getCurrIteration()

string IvPFuncViewerX::getCurrIteration() const
{
  return(m_active_ipf_iter);
}
  
//-------------------------------------------------------------
// Procedure: getCurrTime()

double IvPFuncViewerX::getCurrTime() const
{
  if(m_time_format_utc)
    return(m_curr_time);
  else
    return(m_curr_time - m_min_time_utc);
}


//-------------------------------------------------------------
// Procedure: updateIPF()

void IvPFuncViewerX::updateIPF()
{
  if(strBegins(m_source, "collective"))
    buildCollectiveIPF(m_source);
  else
    buildIndividualIPF();
  
  setIterIPF(uintToString(m_curr_iter)); // Cache the string representation
  redraw();
}

//-------------------------------------------------------------
// Procedure: buildIndividualIPF

void IvPFuncViewerX::buildIndividualIPF(string source)
{
  if(m_map_ipf_plots.count(m_source) == 0)
    return;
  const IPF_TPlot& ipf_plot = m_map_ipf_plots[m_source];

  IvPDomain ivp_subdomain = ipf_plot.getDomainByHelmIter(m_curr_iter);
  setSubDomainIPF(ivp_subdomain);

  string ipf_string = ipf_plot.getIPFByHelmIteration(m_curr_iter);

  if(ipf_string == "") {
    m_quadset = QuadSet();
    return;
  }

  unsigned int pcs  = ipf_plot.getPcsByHelmIteration(m_curr_iter);
  double pwt        = ipf_plot.getPwtByHelmIteration(m_curr_iter);
  setIterIPF(intToString(m_curr_iter));
  setPiecesIPF(intToString(pcs));
  setPriorityIPF(dstringCompact(doubleToString(pwt)));

  IvPDomain ivp_domain = ipf_plot.getIvPDomain();
   
  IvPFunction *ipf = StringToIvPFunction(ipf_string);
  if(ipf) {
    ipf = expandHdgSpdIPF(ipf, ivp_domain);
    bool ok = m_quadset.applyIPF(ipf, m_source);
    if(!ok)
      m_quadset = QuadSet();
    delete(ipf);
    m_quadset.normalize(0, 100);
    m_quadset.applyColorMap(m_color_map);	
  }
}

//-------------------------------------------------------------
// Procedure: buildCollectiveIPF

bool IvPFuncViewerX::buildCollectiveIPF(string ctype)
{
  // Phase 3: Get all the IvPFunction strings for the current iteration
  // for the current vehicle, along with the IvPDomain for each ipf.
  vector<string>    ipfs;
  vector<IvPDomain> ivp_domains;
  vector<string>    ipf_sources;

  map<string, IPF_TPlot>::iterator p;
  for(p=m_map_ipf_plots.begin(); p!=m_map_ipf_plots.end(); p++) {
    string ipf_str = p->second.getIPFByHelmIteration(m_curr_iter);
    if(ipf_str != "") {
      IvPDomain ivp_domain = p->second.getIvPDomain();
      IvPDomain ivp_subdomain = p->second.getDomainByHelmIter(m_curr_iter);
      string    ipf_source = m_source;
      
      bool grab = false;
      if(ctype == "collective-depth") {
	if(ivp_subdomain.hasOnlyDomain("depth"))
	  grab = true;
      }
      if(ctype == "collective-hdgspd") {
	if(ivp_subdomain.hasOnlyDomain("course") ||
	   ivp_subdomain.hasOnlyDomain("speed")  ||
	   ivp_subdomain.hasOnlyDomain("course", "speed"))
	  grab = true;
      }
      
      if(grab) {
	ipfs.push_back(ipf_str);
	ivp_domains.push_back(ivp_domain);
	ipf_sources.push_back(ipf_source);
      }
    }
  }
  
  // If there are no IPFs return false
  if(ipfs.size() == 0) 
    return(false);

  // Phase 4: Build the collective of the given functions.
  m_quadset = QuadSet();

  unsigned int i, vsize = ipfs.size();
  for(i=0; i<vsize; i++) {
    // We grab the IvPDomain associated with each IPF string and pass
    // it along to the setQuadSetFromIPF function. This domain should
    // be for the domain of the helm the produced this IPF, not 
    // necessarily the domain of given IPF, which may be a subdomain.
    IvPDomain ivp_domain = ivp_domains[i];

    QuadSet      quadset;
    IvPFunction *ipf = StringToIvPFunction(ipfs[i]);
    string       src = ipf_sources[i];

    if(ctype == "collective-hdgspd")
      ipf = expandHdgSpdIPF(ipf, ivp_domain);

    if(ipf) {
      quadset.applyIPF(ipf, src);
      delete(ipf);
    }
    m_quadset.addQuadSet(quadset);
  }

  if(ctype == "collective-hdgspd") {
    m_quadset.normalize(0, 100);
    m_quadset.applyColorMap(m_color_map);
  }

  // Phase 5: Set the text information for display
  setIterIPF(intToString(m_curr_iter));
  setPiecesIPF("n/a");
  setPriorityIPF("n/a");
  return(true);
}

//-------------------------------------------------------------
// Procedure: setIterationFromTimeUTC

void IvPFuncViewerX::setIterationFromTimeUTC(double utc_time)
{
  // Assume until proven otherwise that the given time is not
  // contained in any of the IvP plots.
  m_curr_time      = utc_time;
  m_curr_iter      = 0;

  if(!m_iter_plot.containsTimeUTC(utc_time))
    return;

  m_curr_iter = m_iter_plot.getIterByTimeUTC(m_curr_time);
}


//-------------------------------------------------------------
// Procedure: setTimeUTCFromIteration

void IvPFuncViewerX::setTimeUTCFromIteration(unsigned int new_iter)
{
  unsigned int min_iter = m_iter_plot.getMinIter();
  unsigned int max_iter = m_iter_plot.getMaxIter();

  if(new_iter < min_iter)
    new_iter = min_iter;
  if(new_iter > max_iter)
    new_iter = max_iter;

  unsigned int index = m_iter_plot.getIndexByIter(new_iter);
  m_curr_time = m_iter_plot.getTimeUTCByIndex(index);
  m_curr_iter = new_iter;
}

//-------------------------------------------------------------
// Procedure: getPostingBuffer

PostingBuffer IvPFuncViewerX::getPostingBuffer(bool clear)
{
  PostingBuffer return_buffer = m_pbuffer;
  if(clear)
    m_pbuffer.clear();
  return(return_buffer);
}

