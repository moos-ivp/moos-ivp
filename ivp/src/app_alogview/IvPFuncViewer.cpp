/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  m_rad_extra     = 1;
  m_draw_frame    = true;
  m_zoom          = 2.0;
  m_mouse_infocus = false;

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

  if(result && (m_quadset.getQuadSetDim() == 2)) {
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

void IvPFuncViewer::addIPF_Plot(const IPF_Plot& g_ipf_plot)
{
  string ipf_source = g_ipf_plot.getSource();
  string ipf_vname  = g_ipf_plot.getVName();

  m_ipf_vname.push_back(ipf_vname);
  m_ipf_source.push_back(ipf_source);
  m_ipf_plot.push_back(g_ipf_plot);
}

//-------------------------------------------------------------
// Procedure: resetPlotIndex
//      Note: Called when the timestamp/iteration may have changed,
//            but not the preference of ipf type.

void IvPFuncViewer::resetPlotIndex()
{
  if(m_collective_vname != "") {
    bool ok = buildCollectiveIPF(m_collective_vname, m_collective_type);
    if(ok)
      return;
  }
  buildIndividualIPF();
  m_collective_vname = "";
  m_collective_type  = "";
}


//-------------------------------------------------------------
// Procedure: setPlotIndex

void IvPFuncViewer::setPlotIndex(string vname, string source)
{
  m_collective_vname = "";
  m_collective_type  = "";

  if(source == "reset")
    source = m_vname_memory[vname];

  bool built_collective = false;
  if(strBegins(source,"collective"))
    built_collective = buildCollectiveIPF(vname, source);
  
  if(built_collective) {
    m_collective_vname = vname;
    m_collective_type  = source;
  }
  else
    buildIndividualIPF(vname, source);

  
  // remember this for switching back/forth between vehicles
  m_vname_memory[vname] = source;
}

//-------------------------------------------------------------
// Procedure: buildIndividualIPF

void IvPFuncViewer::buildIndividualIPF(string vname, string source)
{
  // Check extreme case - nothing know about any vehicle
  if(m_ipf_vname.size() == 0)
    return;

  bool use_previous_index = false;
  if((vname=="") && (source==""))
    use_previous_index = true;

  if(!use_previous_index) {
    bool found_vname_source = false;
    unsigned int i, vsize = m_ipf_vname.size();
    for(i=0; i<vsize; i++) {
      
      if((tolower(vname) == tolower(m_ipf_vname[i])) && 
	 (tolower(source) == tolower(m_ipf_source[i]))) {
	found_vname_source = true;
	m_plot_ix = i;
      }
    }
    if(!found_vname_source)
      return;
  }

  // Begin update the ivp_subdomain
  unsigned int iter = m_viter_map[m_ipf_vname[m_plot_ix]];
  IvPDomain ivp_subdomain = m_ipf_plot[m_plot_ix].getDomainByHelmIter(iter);
  setSubDomainIPF(ivp_subdomain);

  string ipf_string = m_ipf_plot[m_plot_ix].getIPFByHelmIteration(iter);
  unsigned int pcs = m_ipf_plot[m_plot_ix].getPcsByHelmIteration(iter);
  double pwt = m_ipf_plot[m_plot_ix].getPwtByHelmIteration(iter);
  setIterIPF(intToString(iter));
  setPiecesIPF(intToString(pcs));
  setPriorityIPF(dstringCompact(doubleToString(pwt)));

  IvPDomain ivp_domain = m_ipf_plot[m_plot_ix].getIvPDomain();
  
  if(ipf_string == "") {
    m_quadset = QuadSet();
    return;
  }

  IvPFunction *ipf = StringToIvPFunction(ipf_string);
  if(ipf) {
    ipf = expandHdgSpdIPF(ipf, ivp_domain);
    string ipf_source = m_ipf_source[m_plot_ix];
    bool ok = m_quadset.applyIPF(ipf, ipf_source);
    if(!ok)
      m_quadset = QuadSet();
    delete(ipf);
    m_quadset.normalize(0, 100);
    m_quadset.applyColorMap(m_color_map);	
  }
}

//-------------------------------------------------------------
// Procedure: buildCollectiveIPF

bool IvPFuncViewer::buildCollectiveIPF(string vname, string ctype)
{
  // Phase 1: confirm the vname is known in the vehicle->iter map
  if(!m_viter_map.count(vname))
    return(false);

  // Phase 2: Determine the current helm iteration
  unsigned int curr_iter = m_viter_map[vname];

  // Phase 3: Get all the IvPFunction strings for the current iteration
  // for the current vehicle, along with the IvPDomain for each ipf.
  vector<string>    ipfs;
  vector<IvPDomain> ivp_domains;
  vector<string>    ipf_sources;

  unsigned int i, vsize = m_ipf_plot.size();
  for(i=0; i<vsize; i++) {
    if(m_ipf_vname[i] == vname) {
      string ipf_str = m_ipf_plot[i].getIPFByHelmIteration(curr_iter);
      if(ipf_str != "") {
	IvPDomain ivp_domain = m_ipf_plot[i].getIvPDomain();
	IvPDomain ivp_subdomain = m_ipf_plot[i].getDomainByHelmIter(curr_iter);
	string    ipf_source = m_ipf_source[i];

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
  }
  
  // If there are no IPFs associated with this vname return false
  if(ipfs.size() == 0) 
    return(false);

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
  setIterIPF(intToString(curr_iter));
  setPiecesIPF("n/a");
  setPriorityIPF("n/a");
  return(true);
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
  

  

