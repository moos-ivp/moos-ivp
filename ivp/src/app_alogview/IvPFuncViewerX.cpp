/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPFuncViewerX.cpp                                   */
/*    DATE: Feb 25th, 2007                                       */
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
  m_rad_extra   = 1;
  m_zoom        = 2.0;
  m_curr_time   = 0;
  m_curr_iter   = 0; 
 
  m_mutable_text_size = 10;
 
  setParam("reset_view", "2");

  m_clear_color.setColor("0.6,0.7,0.5");
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: draw()

void IvPFuncViewerX::draw()
{
  Common_IPFViewer::draw();

  if((m_quadset.size2D() != 0) || (m_quadset.size1D() != 0)) {
    glPushMatrix();
    glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);
    
    //Common_IPFViewer::drawFrame();
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
  
  ColorPack cpack("black");
  drawText2(5, h()-15, m_scope_a, cpack, m_mutable_text_size);
  drawText2(5, 5, m_scope_b, cpack, m_mutable_text_size);
}

//-------------------------------------------------------------
// Procedure: resize()

void IvPFuncViewerX::resize(int gx, int gy, int gw, int gh)
{
  return;  // Invoked solely by the parent
  //Common_IPFViewer::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: handle()

int IvPFuncViewerX::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}


//-------------------------------------------------------------
// Procedure: setDataBroker()

void IvPFuncViewerX::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;
}

//-------------------------------------------------------------
// Procedure: setVarPlotA()

void IvPFuncViewerX::setVarPlotA(unsigned int mix)
{
  if(mix < m_dbroker.sizeMix()) {
    m_map_scope_var_a[m_source] = m_dbroker.getVarPlot(mix);
    updateScope();
  }
}

//-------------------------------------------------------------
// Procedure: clearVarPlotA()

void IvPFuncViewerX::clearVarPlotA()
{
  VarPlot null_plot;
  m_map_scope_var_a[m_source] = null_plot;
  updateScope();
}

//-------------------------------------------------------------
// Procedure: setVarPlotB()

void IvPFuncViewerX::setVarPlotB(unsigned int mix)
{
  if(mix < m_dbroker.sizeMix()) {
    m_map_scope_var_b[m_source] = m_dbroker.getVarPlot(mix);
    updateScope();
  }
}

//-------------------------------------------------------------
// Procedure: clearVarPlotB()

void IvPFuncViewerX::clearVarPlotB()
{
  VarPlot null_plot;
  m_map_scope_var_b[m_source] = null_plot;
  updateScope();
}

//-------------------------------------------------------------
// Procedure: setBix

void IvPFuncViewerX::setBix(unsigned int bix)
{
  if(bix == 4900) 
    m_source = "collective-hdgspd";
  else if(bix == 4901) 
    m_source = "collective-depth";
  else {
    string bname = m_dbroker.getBNameFromBix(bix);
    m_source = bname;
  }
  updateIPF();
}


//-------------------------------------------------------------
// Procedure: setIPF_Plots

void IvPFuncViewerX::setIPF_Plots(vector<string> bhv_names)
{
  for(unsigned int i=0; i<bhv_names.size(); i++) {
    string bname = bhv_names[i];
    unsigned int aix = m_dbroker.getAixFromVName(m_vname);
    IPF_Plot iplot = m_dbroker.getIPFPlot(aix, bname);
    if(iplot.size() > 0)
      addIPF_Plot(iplot);
    else
      cout << "Warning: Broker returned empty IPF_Plot." << endl;
  }
}

//-------------------------------------------------------------
// Procedure: setHelmIterPlot

void IvPFuncViewerX::setHelmIterPlot()
{
  if(m_vname == "")
    return;
  
  unsigned int mix = m_dbroker.getMixFromVNameVarName(m_vname, "IVPHELM_ITER");

  m_iter_plot = m_dbroker.getLogPlot(mix);
}

//-------------------------------------------------------------
// Procedure: addIPF_Plot
//      Note: A local copy of the given IPF_Plot is created here.

void IvPFuncViewerX::addIPF_Plot(const IPF_Plot& g_ipf_plot)
{
  string ipf_source = g_ipf_plot.getSource();
  string ipf_vname  = g_ipf_plot.getVName();

  if(m_vname != ipf_vname) {
    cout << "name_mismatch" << endl;
    cout << "m_vname: " << m_vname << ", ipf_vname: [" << ipf_vname << "]" << endl;
    return;
  }

  m_map_ipf_plots[ipf_source] = g_ipf_plot;
  if(m_source == "")
    m_source = ipf_source;
}

//-------------------------------------------------------------
// Procedure: setTime()

void IvPFuncViewerX::setTime(double time)
{
  m_curr_time = time;
  m_curr_iter = (unsigned int)(m_iter_plot.getValueByTime(m_curr_time));  
  updateIPF();

  updateScope();
}


//-------------------------------------------------------------
// Procedure: updateScope

void IvPFuncViewerX::updateScope()
{
  m_scope_a = "";
  m_scope_b = "";

  if(m_map_scope_var_a.count(m_source))
    m_scope_a = m_map_scope_var_a[m_source].getEntryByTime(m_curr_time);

  if(m_map_scope_var_b.count(m_source))
    m_scope_b = m_map_scope_var_b[m_source].getEntryByTime(m_curr_time);
}


//-------------------------------------------------------------
// Procedure: setSource

void IvPFuncViewerX::setSource(string source)
{
  if(m_map_ipf_plots.count(source) || strBegins(source, "collective"))
    m_source = source;
  updateIPF();
  updateScope();
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
  return(m_curr_time);
}

//-------------------------------------------------------------
// Procedure: getCurrPriority(string)

string IvPFuncViewerX::getCurrPriority(string source) 
{
  if(m_map_ipf_plots.count(source) == 0)
    return("--");

  const IPF_Plot& ipf_plot = m_map_ipf_plots[source];
  double pwt = ipf_plot.getPwtByHelmIteration(m_curr_iter);
  
  string result = doubleToString(pwt, 3);

  return(result);  
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

bool IvPFuncViewerX::buildIndividualIPF(string source)
{
  if(m_map_ipf_plots.count(m_source) == 0)
    return(false);

  const IPF_Plot& ipf_plot = m_map_ipf_plots[m_source];

  IvPDomain ivp_subdomain = ipf_plot.getDomainByHelmIter(m_curr_iter);
  setSubDomainIPF(ivp_subdomain);

  //ivp_subdomain.print();

  string ipf_string = ipf_plot.getIPFByHelmIteration(m_curr_iter);

  if(ipf_string == "") {
    m_quadset = QuadSet();
    setPiecesIPF("n/a");
    setPriorityIPF("n/a");
    return(false);
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
  return(true);
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

  map<string, IPF_Plot>::iterator p;
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
  if(ipfs.size() == 0) {
    clear();
    m_quadset = QuadSet();
    return(false);
  }

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

