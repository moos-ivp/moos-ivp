/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: EncounterViewer.cpp                                  */
/*    DATE: Jan 11th, 2016                                       */
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
#include "EncounterViewer.h"
#include "FunctionEncoder.h"
#include "IPFViewUtils.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "MBTimer.h"

using namespace std;

EncounterViewer::EncounterViewer(int x, int y, int w, int h, const char *l)
  : Fl_Gl_Window(x, y, w, h, l)
{
  m_curr_time   = 0;
  m_curr_iter   = 0; 
 
  m_mutable_text_size = 10;
 
  m_clear_color.setColor("0.6,0.7,0.5");
  m_label_color.setColor("brown");
}

//-------------------------------------------------------------
// Procedure: draw()

void EncounterViewer::draw()
{
#if 0
  //=================================================================
  clear();

  glViewport(0,0,w(),h());
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(0.0, 0.25, 0.0);
  glScalef(m_zoom, m_zoom, m_zoom);

  GLfloat nRange = 750.0f;

  // Reset projection matrix stack
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Establish clipping volume (left, right, bottom, top, near, far)
  glOrtho (-nRange, nRange, -nRange*h()/w(), nRange*h()/w(), -nRange, nRange);

  // Set color shading model to flat
  glShadeModel(GL_FLAT);
  
  // Clock wise wound polygons are front facing, this is reversed
  // because we are using triangle fans
  glFrontFace(GL_CW);

  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //=================================================================

  glPopMatrix();
  glFlush();
  
  ColorPack cpack("black");
  drawText2(5, h()-15, m_scope_a, cpack, m_mutable_text_size);
  drawText2(5, 5, m_scope_b, cpack, m_mutable_text_size);

#endif
}

//-------------------------------------------------------------
// Procedure: resize()

void EncounterViewer::resize(int gx, int gy, int gw, int gh)
{
  return;  // Invoked solely by the parent
  //Common_IPFViewer::resize(gx, gy, gw, gh);
}

//-------------------------------------------------------------
// Procedure: handle()

int EncounterViewer::handle(int event)
{
  return(Fl_Gl_Window::handle(event));
}


//-------------------------------------------------------------
// Procedure: setDataBroker()

void EncounterViewer::setDataBroker(ALogDataBroker dbroker, string vname)
{
  m_dbroker = dbroker;
  m_vname   = vname;
}

//-------------------------------------------------------------
// Procedure: setVarPlotA()

void EncounterViewer::setVarPlotA(unsigned int mix)
{
  if(mix < m_dbroker.sizeMix()) {
    m_map_scope_var_a[m_source] = m_dbroker.getVarPlot(mix);
    updateScope();
  }
}

//-------------------------------------------------------------
// Procedure: clearVarPlotA()

void EncounterViewer::clearVarPlotA()
{
  VarPlot null_plot;
  m_map_scope_var_a[m_source] = null_plot;
  updateScope();
}

//-------------------------------------------------------------
// Procedure: setVarPlotB()

void EncounterViewer::setVarPlotB(unsigned int mix)
{
  if(mix < m_dbroker.sizeMix()) {
    m_map_scope_var_b[m_source] = m_dbroker.getVarPlot(mix);
    updateScope();
  }
}

//-------------------------------------------------------------
// Procedure: clearVarPlotB()

void EncounterViewer::clearVarPlotB()
{
  VarPlot null_plot;
  m_map_scope_var_b[m_source] = null_plot;
  updateScope();
}

//-------------------------------------------------------------
// Procedure: setBix

void EncounterViewer::setBix(unsigned int bix)
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

void EncounterViewer::setIPF_Plots(vector<string> bhv_names)
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

void EncounterViewer::setHelmIterPlot()
{
  if(m_vname == "")
    return;
  
  unsigned int mix = m_dbroker.getMixFromVNameVarName(m_vname, "IVPHELM_ITER");

  m_iter_plot = m_dbroker.getLogPlot(mix);
}

//-------------------------------------------------------------
// Procedure: addIPF_Plot
//      Note: A local copy of the given IPF_Plot is created here.

void EncounterViewer::addIPF_Plot(const IPF_Plot& g_ipf_plot)
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

void EncounterViewer::setTime(double time)
{
  m_curr_time = time;
  m_curr_iter = (unsigned int)(m_iter_plot.getValueByTime(m_curr_time));  
  updateIPF();

  updateScope();
}


//-------------------------------------------------------------
// Procedure: updateScope

void EncounterViewer::updateScope()
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

void EncounterViewer::setSource(string source)
{
  if(m_map_ipf_plots.count(source) || strBegins(source, "collective"))
    m_source = source;
  updateIPF();
  updateScope();
}


//-------------------------------------------------------------
// Procedure: getCurrPieces()

string EncounterViewer::getCurrPieces() const
{
  return("");
}
  
//-------------------------------------------------------------
// Procedure: getCurrPriority()

string EncounterViewer::getCurrPriority() const
{
  return("");
}
  
//-------------------------------------------------------------
// Procedure: getCurrDomain()

string EncounterViewer::getCurrDomain() const
{
  return("");
}
  
//-------------------------------------------------------------
// Procedure: getCurrIteration()

string EncounterViewer::getCurrIteration() const
{
  return("");
}
  
//-------------------------------------------------------------
// Procedure: getCurrTime()

double EncounterViewer::getCurrTime() const
{
  return(m_curr_time);
}

//-------------------------------------------------------------
// Procedure: getCurrPriority(string)

string EncounterViewer::getCurrPriority(string source) 
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

void EncounterViewer::updateIPF()
{

}

//-------------------------------------------------------------
// Procedure: buildIndividualIPF

bool EncounterViewer::buildIndividualIPF(string source)
{
  return(true);
}

//-------------------------------------------------------------
// Procedure: buildCollectiveIPF

bool EncounterViewer::buildCollectiveIPF(string ctype)
{
  return(true);
}

