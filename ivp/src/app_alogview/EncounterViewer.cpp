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
#include "MBUtils.h"
#include "MBTimer.h"

using namespace std;

EncounterViewer::EncounterViewer(int x, int y, int w, int h, const char *l)
  : Fl_Gl_Window(x, y, w, h, l)
{
  m_curr_time   = 0;
 
  m_mutable_text_size = 10;
 
  m_clear_color.setColor("0.95,0.95,0.95");
  m_label_color.setColor("brown");
  m_mineff_color.setColor("red");
  m_avgeff_color.setColor("darkblue");
  m_mincpa_color.setColor("red");
  m_avgcpa_color.setColor("darkblue");

  m_draw_mineff = false;
  m_draw_avgeff = false;
  m_draw_mincpa = false;
  m_draw_avgcpa = false;

  m_cpa_collision = 8;
  m_cpa_near_miss = 10;

  m_show_allpts = false;

  m_min_cpa = 0;
  m_min_eff = 0;
}

//-------------------------------------------------------------
// Procedure: draw()

void EncounterViewer::draw()
{
  if(m_encounter_plot.size() == 0)
    return;

  cout << "In EncounterViewer::draw()  " << "w:" << w() << ", h:" << h() << endl;
  //cout << "vname: " << m_vname << endl;
  //cout << "plotsize: " << m_encounter_plot.size() << endl;
  
  double max_cpa = m_encounter_plot.getMaxCPA();

  //cout << "max_cpa: " << max_cpa << endl;
  
  vector<double> v_cpa_pix;
  vector<double> v_eff_pix;
  for(unsigned int i=0; i<m_encounter_plot.size(); i++) {
    double time = m_encounter_plot.getTimeByIndex(i);
    if((time > m_curr_time) && !m_show_allpts)
      break;
    
    double eff_pct = m_encounter_plot.getValueEffByIndex(i);
    double eff_pix = (eff_pct/100) * h();
    
    double cpa = m_encounter_plot.getValueCPAByIndex(i);
    double cpa_pct = cpa / max_cpa; 
    double cpa_pix = cpa_pct * w();

    //cout << "cpa [" << i << "]:" << cpa << endl;
    v_eff_pix.push_back(eff_pix);
    v_cpa_pix.push_back(cpa_pix);
  }
  
  glClearColor(m_clear_color.red(),m_clear_color.grn(),m_clear_color.blu(),0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, w(), h());
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();


  // Draw the collision zone
  double cpax_pct = m_cpa_collision / max_cpa; 
  double cpax_pix = cpax_pct * w();
  glEnable(GL_BLEND);
  glColor4f(0.9, 0.7, 0.7, 0.4);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POLYGON);
  glVertex2f(0, 0);
  glVertex2f(0, h());
  glVertex2f(cpax_pix, h());
  glVertex2f(cpax_pix, 0);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_BLEND);

  // Draw the near_miss zone
  double cpan_pct = m_cpa_near_miss / max_cpa; 
  double cpan_pix = cpan_pct * w();
  glEnable(GL_BLEND);
  glColor4f(0.9, 0.9, 0.7, 0.4);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POLYGON);
  glVertex2f(cpax_pix, 0);
  glVertex2f(cpax_pix, h());
  glVertex2f(cpan_pix, h());
  glVertex2f(cpan_pix, 0);
  glVertex2f(cpax_pix, 0);
  glEnd();
  glDisable(GL_BLEND);

  // Draw the low-efficiency zone
  glEnable(GL_BLEND);
  glColor4f(0.8, 0.8, 0.8, 0.3);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_POLYGON);
  glVertex2f(0, 0);
  glVertex2f(0, h()/2);
  glVertex2f(w(), h()/2);
  glVertex2f(w(), 0);
  glVertex2f(0, 0);
  glEnd();
  glDisable(GL_BLEND);

  // Draw the near_miss zone

  glEnable(GL_POINT_SMOOTH);
  // Draw the first N-1 points in one color
  glPointSize(5);
  glColor3f(0.4, 0.4, 0.5); 
  glBegin(GL_POINTS);
  for(unsigned int i=0; i<v_cpa_pix.size()-1; i++) 
    glVertex2f(v_cpa_pix[i], v_eff_pix[i]);
  glEnd();

  // Draw the Nth point in perhaps different color
  glColor3f(0.5, 0.8, 0.5); 
  glPointSize(10);
  glBegin(GL_POINTS);
  unsigned int ix = v_cpa_pix.size()-1;
  glVertex2f(v_cpa_pix[ix], v_eff_pix[ix]);
  glEnd();
  glDisable(GL_POINT_SMOOTH);


  // Draw the Min Efficiency Line
  if(m_draw_mineff) {
    double min_eff = m_encounter_plot.getMinEFF();
    double min_eff_pix = (min_eff / 100) * h();
    glColor3f(m_mineff_color.red(), m_mineff_color.grn(), m_mineff_color.blu());
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,  min_eff_pix);
    glVertex2f(w(), min_eff_pix);
    glEnd();
  }

  // Draw the Avg Efficiency Line
  if(m_draw_avgeff) {
    double avg_eff = m_encounter_plot.getMeanEFF();
    double avg_eff_pix = (avg_eff / 100) * h();
    glColor3f(m_avgeff_color.red(), m_avgeff_color.grn(), m_avgeff_color.blu());
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,  avg_eff_pix);
    glVertex2f(w(), avg_eff_pix);
    glEnd();
  }

  // Draw the Min CPA Line
  if(m_draw_mincpa) {
    double min_cpa = m_encounter_plot.getMinCPA();
    double min_cpa_pct = min_cpa / max_cpa; 
    double min_cpa_pix = min_cpa_pct * w();
    glColor3f(m_mincpa_color.red(), m_mincpa_color.grn(), m_mincpa_color.blu());
    glBegin(GL_LINE_STRIP);
    glVertex2f(min_cpa_pix, 0);
    glVertex2f(min_cpa_pix, h());
    glEnd();
  }

  // Draw the Avg CPA Line
  if(m_draw_avgcpa) {
    double avg_cpa = m_encounter_plot.getMeanCPA();
    double avg_cpa_pct = avg_cpa / max_cpa; 
    double avg_cpa_pix = avg_cpa_pct * w();
    glColor3f(m_avgcpa_color.red(), m_avgcpa_color.grn(), m_avgcpa_color.blu());
    glBegin(GL_LINE_STRIP);
    glVertex2f(avg_cpa_pix, 0);
    glVertex2f(avg_cpa_pix, h());
    glEnd();
  }




  
  glFlush();
  glPopMatrix();
  
  //ColorPack cpack("black");
  //drawText2(5, h()-15, m_scope_a, cpack, m_mutable_text_size);
  //drawText2(5, 5, m_scope_b, cpack, m_mutable_text_size);
}

//-------------------------------------------------------------
// Procedure: resize()

void EncounterViewer::resize(int gx, int gy, int gw, int gh)
{
  Fl_Gl_Window::resize(gx, gy, gw, gh);
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

  unsigned int aix = dbroker.getAixFromVName(vname);
  m_encounter_plot = dbroker.getEncounterPlot(aix);

  m_min_cpa = m_encounter_plot.getMinCPA();
  m_min_eff = m_encounter_plot.getMinEFF();
}

//-------------------------------------------------------------
// Procedure: setTime()

void EncounterViewer::setTime(double time)
{
  m_curr_time = time;
}


//-------------------------------------------------------------
// Procedure: getTotalEncounters()

string EncounterViewer::getTotalEncounters() const
{
  unsigned int total_encounters = m_encounter_plot.size();
  return(uintToString(total_encounters));
}
  
//-------------------------------------------------------------
// Procedure: getCurrTime()

double EncounterViewer::getCurrTime() const
{
  return(m_curr_time);
}

