/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IC_Viewer.cpp                                        */
/*    DATE: Apr 19th 2011                                        */
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
#include "IC_Viewer.h"
#include "ColorParse.h"
#include "BuildUtils.h"
#include "AngleUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

IC_Viewer::IC_Viewer(int x, int y, int wid, int hgt, const char *l)
  : Common_IPFViewer(x, y, wid, hgt, l)
{
  m_clear_color.setColor("macbeige");
  m_collective  = false;
  m_curr_iter   = 0;
  m_xRot        = 0;
  m_zRot        = 0;
  m_rad_extra   = 27;
}

//-------------------------------------------------------------
// Procedure: setIvPChecker

void IC_Viewer::setIvPChecker(IvPChecker ivp_checker)
{
  m_ivp_checker = ivp_checker;
}

//-------------------------------------------------------------
// Procedure: setCurrIteration

void IC_Viewer::setCurrIteration(unsigned int iter)
{
  m_curr_iter = iter;

  // If the bundle_series is empty, don't clip the iteration
  if(m_bundle_series.size() == 0)
    return;

  // If the bundle_series is non-empty, clip the iteration

  if(m_curr_iter < m_bundle_series.getMinIteration())
    m_curr_iter = m_bundle_series.getMinIteration();
  if(m_curr_iter > m_bundle_series.getMaxIteration())
    m_curr_iter = m_bundle_series.getMaxIteration();

  // Reset the domain and the focus point. The domain may change from
  // one interation to the next. So reset each time.
  m_ivp_domain  = m_bundle_series.getDomain(m_curr_iter);
  unsigned int dim = m_ivp_domain.size();
  m_focus_point = IvPBox(dim);
}

//-------------------------------------------------------------
// Procedure: draw

void IC_Viewer::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);
  
  bool result = Common_IPFViewer::drawIvPFunction();
  if(result) {
    if(m_draw_frame)
      drawFrame();
    drawOwnPoint();

    unsigned int max_crs_qix = m_quadset.getMaxPointQIX("course");
    unsigned int max_spd_qix = m_quadset.getMaxPointQIX("speed");
    drawMaxPoint(max_crs_qix, max_spd_qix);

  }
  glPopMatrix();
  
  glFlush();  
}

//-------------------------------------------------------------
// Procedure: handle()

int IC_Viewer::handle(int event)
{
  if((m_xRot != 0) || (m_zRot != 0))
    return(Fl_Gl_Window::handle(event));

  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handleLeftMouse(vx, vy);
    }
    return(1);
    break;  
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse()

void IC_Viewer::handleLeftMouse(int vx, int vy)
{
  cout << "In IC_Viewer::handleLeftMouse(): vx:" << vx << " vy:" << vy << endl;

  double cx = w() / 2;
  double cy = h() / 2;

  cout << "Center: cx:" << cx << " cy:" << cy << endl;

  double angle = angle360(relAng(cx, cy, vx, vy));
  cout << "Angle: " << angle << endl;
 
  unsigned int spd_pts = m_ivp_domain.getVarPoints("speed");
  
  double mod_pts = (double)(spd_pts-1) * m_rad_extra;

  double dist = sqrt(((cx-vx)*(cx-vx)) + ((cy-vy)*(cy-vy)));

  dist *= 2.03; // HACK mikerb
  
  double pct = (dist / mod_pts) * 100;
  cout << "m_rad_extra:" << m_rad_extra << endl;
  cout << "m_zoom:" << m_zoom << endl;
  cout << "dist:" << dist << endl;
  cout << "pct:" << pct << endl;

  double speed = (pct/100) * m_ivp_domain.getVarHigh("speed");

  setFocusPoint("course", angle);
  setFocusPoint("speed", speed);
}


//-------------------------------------------------------------
// Procedure: checkIvPSolve

void IC_Viewer::checkIvPSolve()
{
  cout << termColor("green");
  cout << "IvP Solution SUMMARY---------------------" << endl;
  
  vector<string> ipf_strings = m_bundle_series.getIPFStrings(m_curr_iter);
  
  cout << "For Iteration:          " << m_curr_iter << endl;
  cout << "Total # of IPF strings: " << ipf_strings.size() << endl;
  
  m_ivp_checker.checkIvP(ipf_strings);
  cout << termColor() << endl;
}

//-------------------------------------------------------------
// Procedure: checkBruteSolve()

void IC_Viewer::checkBruteSolve()
{
  cout << termColor("blue");
  cout << "Brute Force Solution SUMMARY---------------------" << endl;
  
  vector<string> ipf_strings = m_bundle_series.getIPFStrings(m_curr_iter);
  
  cout << "For Iteration:          " << m_curr_iter << endl;
  cout << "Total # of IPF strings: " << ipf_strings.size() << endl;
  
  m_ivp_checker.checkBrute(ipf_strings);
  cout << termColor() << endl;
}

//-------------------------------------------------------------
// Procedure: checkQuads

void IC_Viewer::checkQuads()
{
  cout << termColor("magenta");
  cout << "Quadset calculation SUMMARY---------------------" << endl;
  cout << "For Iteration: " << m_curr_iter << endl;
  
  QuadSet collective_quadset;
  collective_quadset = m_bundle_series.getCollectiveQuadSet(m_curr_iter);
  m_ivp_checker.checkCollectiveQuadSet(collective_quadset);
  cout << termColor() << endl;
}

//-------------------------------------------------------------
// Procedure: setIPF_BundleSeries

void IC_Viewer::setIPF_BundleSeries(IPF_BundleSeries bundle_series)
{
  m_bundle_series = bundle_series;
  vector<string> all_sources = m_bundle_series.getAllSources();
  if(all_sources.size() > 0)
    m_curr_source = all_sources[0];

  if(m_curr_iter < m_bundle_series.getMinIteration())
    m_curr_iter = m_bundle_series.getMinIteration();
  if(m_curr_iter > m_bundle_series.getMaxIteration())
    m_curr_iter = m_bundle_series.getMaxIteration();
  
  // Reset the domain and the focus point. The domain may change from
  // one interation to the next. So reset each time.
  m_ivp_domain  = m_bundle_series.getDomain(m_curr_iter);
  unsigned int dim = m_ivp_domain.size();
  m_focus_point = IvPBox(dim);
}

//-------------------------------------------------------------
// Procedure: resetQuadSet

void IC_Viewer::resetQuadSet()
{
  if(m_collective) {
    m_quadset = m_bundle_series.getCollectiveQuadSet(m_curr_iter);
    if(m_quadset.isEmptyND())
      return;
    m_quadset.normalize(0,200);
    m_quadset.applyColorMap(m_color_map);
  }
  else {
    m_quadset = m_bundle_series.getQuadSet(m_curr_iter, m_curr_source);
    if(m_quadset.isEmptyND())
      return;
    m_quadset.normalize(0,200);
    m_quadset.applyColorMap(m_color_map);
  }
}


//-------------------------------------------------------------
// Procedure: setFocusPoint()

void IC_Viewer::setFocusPoint(string var, double val)
{
  cout << "Setting Focus Point: var:" << var << " val:" << val << endl;
  if(!m_ivp_domain.hasDomain(var))
    return;

  
  
  int    dom_ix  = m_ivp_domain.getIndex(var);

  double var_low = m_ivp_domain.getVarLow(dom_ix);
  double var_hgh = m_ivp_domain.getVarHigh(dom_ix);
  double var_inc = m_ivp_domain.getVarDelta(dom_ix);
  double var_pts = m_ivp_domain.getVarPoints(dom_ix);

  int interval_ix = 0;

  if(val >= var_hgh)
    interval_ix = var_pts-1;
  else if(val <= var_low)
    interval_ix = 0;
  else
    interval_ix = (int)(((val - var_low) / var_inc) + 0.5);

  cout << "interval_ix: " << interval_ix << endl;
  
  if(m_focus_point.getDim() != (int)(m_ivp_domain.size()))
    m_focus_point = IvPBox(m_ivp_domain.size());

  m_focus_point.setPTS(dom_ix, interval_ix, interval_ix);    
}

//-------------------------------------------------------------
// Procedure: evalFocusPoint

void IC_Viewer::evalFocusPoint()
{
  cout << "-------------------------------" << endl;
  cout << "Focus Point: " << endl;
  
  unsigned int d, dim = m_ivp_domain.size();
  for(d=0; d<dim; d++) {
    unsigned int index = m_focus_point.pt(d);
    string var = m_ivp_domain.getVarName(d);
    double val   = m_ivp_domain.getVal(d, index);
    cout << var << ":" << index << ":" << val << endl;
  }
  cout << "-------------------------------" << endl;
}


//-------------------------------------------------------------
// Procedure: getFocusPoint

string IC_Viewer::getFocusPoint()
{
  string str = "";
  //string str = domainAndBoxToString(m_focus_point, m_ivp_domain);
  return(str);
}

//-------------------------------------------------------------
// Procedure: incCurrFunction

void IC_Viewer::incCurrFunction()
{
  vector<string> sources = m_bundle_series.getAllSources();
  unsigned int i, vsize = sources.size();
  if(vsize == 0)
    return;
  
  unsigned int new_index = 0;
  for(i=0; i<vsize; i++) {
    if(sources[i] == m_curr_source) {
      if(i == (vsize-1))
	new_index = 0;
      else
	new_index = i+1;
    }
  }
  m_curr_source = sources[new_index];

  resetQuadSet();
}


//-------------------------------------------------------------
// Procedure: decCurrFunction

void IC_Viewer::decCurrFunction()
{
  vector<string> sources = m_bundle_series.getAllSources();
  unsigned int i, vsize = sources.size();
  if(vsize == 0)
    return;
  
  unsigned int new_index = 0;
  for(i=0; i<vsize; i++) {
    if(sources[i] == m_curr_source) {
      if(i == 0)
	new_index = vsize-1;
      else
	new_index = i-1;
    }
  }
  m_curr_source = sources[new_index];

  resetQuadSet();
}

//-------------------------------------------------------------
// Procedure: getCurrDescriptor()

string IC_Viewer::getCurrDescriptor()
{
  if(m_collective)
    return("Collective");
  else 
    return(m_curr_source);
}

//-------------------------------------------------------------
// Procedure: getCurrPriority()

string IC_Viewer::getCurrPriority()
{
  if(m_collective)
    return("0");
  else {
    double pwt = m_bundle_series.getPriority(m_curr_iter, m_curr_source);
    return(dstringCompact(doubleToString(pwt)));
  }
}

//-------------------------------------------------------------
// Procedure: getCurrDomain()

string IC_Viewer::getCurrDomain()
{
  IvPDomain ivp_domain;
  if(m_collective)
    ivp_domain = m_bundle_series.getDomain(m_curr_iter);
  else
    ivp_domain = m_bundle_series.getDomain(m_curr_iter, m_curr_source);

  return(domainToString(ivp_domain));
}

//-------------------------------------------------------------
// Procedure: getCurrPieces()

string IC_Viewer::getCurrPieces()
{
  if(m_collective)
    return("0");
  else {
    unsigned int pcs;
    pcs = m_bundle_series.getPieces(m_curr_iter, m_curr_source);
    return(dstringCompact(uintToString(pcs)));
  }
}

//-------------------------------------------------------------
// Procedure: getTotalFunctions()

string IC_Viewer::getTotalFunctions() 
{
  unsigned int bsize = m_bundle_series.getTotalFunctions(m_curr_iter);
  return(uintToString(bsize));
}


//-------------------------------------------------------------
// Procedure: setCollective

void IC_Viewer::setCollective(const string& val)
{
  bool prior_collective = m_collective;
  if(val == "true")
    m_collective = true;
  else if(val == "false")
    m_collective = false;
  else if(val == "toggle")
    m_collective = !m_collective;

  if(m_collective != prior_collective)
    resetQuadSet();  

}


