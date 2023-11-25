/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "GUI_Encounters.h"
#include "MBUtils.h"
#include "MBTimer.h"

using namespace std;

EncounterViewer::EncounterViewer(int x, int y, int w, int h, const char *l)
  : Fl_Gl_Window(x, y, w, h, l)
{
  m_curr_time   = 0;
 
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

  m_collision_range = 8.5;
  m_near_miss_range = 10.5;
  m_encounter_range = 50.5;
  
  m_show_allpts = false;

  m_draw_pointsize = 4;
  m_curr_buff_ix = 0;
  
  m_min_cpa = 0;
  m_min_eff = 0;
  m_avg_cpa = 0;
  m_avg_eff = 0;

  m_owning_gui = 0;

  // The use_high_res_GL function is supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  Fl::use_high_res_GL(1);
#endif
}

//-------------------------------------------------------------
// Procedure: setCurrBuffIndex()

void EncounterViewer::setCurrBuffIndex(double cpa_pix, double eff_pix)
{
  if(m_curr_buff_size == 0)
    return;

  double min_delta = 0;
  double min_index = 0;

  for(unsigned int i=0; i<m_buff_cpa_pix.size(); i++) {
    double cpa_delta = (cpa_pix - m_buff_cpa_pix[i]);
    double eff_delta = (eff_pix - m_buff_eff_pix[i]);
    double delta = hypot(cpa_delta, eff_delta);
    if((i==0) || (delta < min_delta)) {
      min_delta = delta;
      min_index = i;
    }
  }

  m_curr_buff_ix = min_index;
}

//-------------------------------------------------------------
// Procedure: refreshDrawBuffers()

void EncounterViewer::refreshDrawBuffers()
{
  if(m_encounter_plot.size() == 0)
    return;

  // Part 1: Clear the buffers
  m_buff_cpa_pix.clear();
  m_buff_eff_pix.clear();
  
  // Part 2: Fill the buffers
  for(unsigned int i=0; i<m_encounter_plot.size(); i++) {
    double time = m_encounter_plot.getTimeByIndex(i);
    if((time > m_curr_time) && !m_show_allpts)
      break;
    
    double eff_pct = m_encounter_plot.getValueEffByIndex(i);
    double eff_pix = (eff_pct/100) * h();
    
    double cpa = m_encounter_plot.getValueCPAByIndex(i);
    double cpa_pct = cpa / m_encounter_range; 
    double cpa_pix = cpa_pct * w();

    m_buff_eff_pix.push_back(eff_pix);
    m_buff_cpa_pix.push_back(cpa_pix);
  }

  // Part 3: If the buffer size has changed, handle it
  if(m_buff_cpa_pix.size() != m_curr_buff_size) {
    // Update the stored buffer size
    m_curr_buff_size = m_buff_cpa_pix.size();
    // If the curr_buff_ix now exceeds the buffer size, adjust
    if(m_curr_buff_size == 0)
      m_curr_buff_ix = 0;
    else if(m_curr_buff_ix >= m_curr_buff_size)
      m_curr_buff_ix = m_curr_buff_size - 1;

    // If we're not showing all pts, curr ix is always reset to
    // latest if the size of the buffer changes
    if(!m_show_allpts)
      m_curr_buff_ix = m_curr_buff_size - 1;
  }
}


    
//-------------------------------------------------------------
// Procedure: draw()

void EncounterViewer::draw()
{
  if(m_encounter_plot.size() == 0)
    return;

  //cout << "In EncounterViewer::draw()  " << "w:" << w() << ", h:" << h() << endl;
  //cout << "vname: " << m_vname << endl;
  //cout << "plotsize: " << m_encounter_plot.size() << endl;

  //cout << "m_collision_range: " << m_collision_range << endl;
  //cout << "m_near_miss_range: " << m_near_miss_range << endl;
  //cout << "m_curr_time:       " << m_curr_time << endl;
  //cout << "m_encounter_range: " << m_encounter_range << endl;
  
  refreshDrawBuffers();

  
  // Prepare to draw
  glClearColor(m_clear_color.red(),m_clear_color.grn(),m_clear_color.blu(),0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // The pixel_w/h() functions are supported in more recent FLTK
  // packages. FLTK on older non-MacOS systems may not have this
  // feature. It is mostly needed to support Mac Retina displays.
#ifdef __APPLE__
  glViewport(0, 0, pixel_w(), pixel_h());
#else
  glViewport(0, 0, w(), h());
#endif

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();


  // Draw the collision zone
  double cpax_pct = m_collision_range / m_encounter_range; 
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
  double cpan_pct = m_near_miss_range / m_encounter_range; 
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
  glPointSize(m_draw_pointsize);
  glColor3f(0.4, 0.4, 0.5); 
  glBegin(GL_POINTS);
  for(unsigned int i=0; i<m_buff_cpa_pix.size(); i++) 
    glVertex2f(m_buff_cpa_pix[i], m_buff_eff_pix[i]);
  glEnd();

  // Draw the Nth point in perhaps different color
  if(m_buff_cpa_pix.size() > 0) {
    glColor3f(0.5, 0.8, 0.5); 
    glPointSize(m_draw_pointsize*2);
    glBegin(GL_POINTS);
    unsigned int ix = m_curr_buff_ix;
    glVertex2f(m_buff_cpa_pix[ix], m_buff_eff_pix[ix]);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
  }

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
    double min_cpa_pct = min_cpa / m_encounter_range; 
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
    double avg_cpa_pct = avg_cpa / m_encounter_range; 
    double avg_cpa_pix = avg_cpa_pct * w();
    glColor3f(m_avgcpa_color.red(), m_avgcpa_color.grn(), m_avgcpa_color.blu());
    glBegin(GL_LINE_STRIP);
    glVertex2f(avg_cpa_pix, 0);
    glVertex2f(avg_cpa_pix, h());
    glEnd();
  }

  glFlush();
  glPopMatrix();
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
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handleLeftMouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handleRightMouse(vx, vy);
    }
    return(1);
  default:
    return(Fl_Gl_Window::handle(event));
  }
  return(Fl_Gl_Window::handle(event));
}


//-------------------------------------------------------------
// Procedure: handleLeftMouse                       

void EncounterViewer::handleLeftMouse(int vx, int vy)
{
  //cout << "EncounterViewer::handleLeftMouse: x:" << vx << ", y:" << vx << endl; 

  double xpct = (double)(vx) / (double)(w());
  double ypct = (double)(vy) / (double)(h());

  //cout << "xpct = " << xpct << endl;
  //cout << "ypct = " << ypct << endl;

  //double mouse_cpa = xpct * m_encounter_range;
  //double mouse_eff = ypct * 100;

  //cout << "mouse_cpa: " << mouse_cpa << endl;
  //cout << "mouse_eff: " << mouse_eff<< endl;

  double mouse_cpa_pix = xpct * w();
  double mouse_eff_pix = ypct * h();

  setCurrBuffIndex(mouse_cpa_pix, mouse_eff_pix);
  redraw();
  if(m_owning_gui) {
    m_owning_gui->updateXY();
    m_owning_gui->setCurrTimeX(getCurrIndexTime());
  }
}

//-------------------------------------------------------------
// Procedure: handleRightMouse                       

void EncounterViewer::handleRightMouse(int vx, int vy)
{
  //cout << "EncounterViewer::handleRightMouse: x:" << vx << ", y:" << vx << endl; 
}



//-------------------------------------------------------------
// Procedure: getCurrIndexTime()

double EncounterViewer::getCurrIndexTime() const
{
  return(m_encounter_plot.getTimeByIndex(m_curr_buff_ix));
}

//-------------------------------------------------------------
// Procedure: getCurrIndexCPA()

double EncounterViewer::getCurrIndexCPA() const
{
  return(m_encounter_plot.getValueCPAByIndex(m_curr_buff_ix));
}

//-------------------------------------------------------------
// Procedure: getCurrIndexEFF()

double EncounterViewer::getCurrIndexEFF() const
{
  return(m_encounter_plot.getValueEffByIndex(m_curr_buff_ix));
}

//-------------------------------------------------------------
// Procedure: getCurrIndexID()

int EncounterViewer::getCurrIndexID() const
{
  return(m_encounter_plot.getValueIDByIndex(m_curr_buff_ix));
}

//-------------------------------------------------------------
// Procedure: getCurrIndexContact()

string EncounterViewer::getCurrIndexContact() const
{
  return(m_encounter_plot.getValueContactByIndex(m_curr_buff_ix));
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
  m_avg_cpa = m_encounter_plot.getMeanCPA();
  m_avg_eff = m_encounter_plot.getMeanEFF();

  m_collision_range = m_encounter_plot.getCollisionRange();
  m_near_miss_range = m_encounter_plot.getNearMissRange();
  m_encounter_range = m_encounter_plot.getEncounterRange();
}

//-------------------------------------------------------------
// Procedure: setOwningGUI()

void EncounterViewer::setOwningGUI(GUI_Encounters *gui)
{
  m_owning_gui = gui;
}

//-------------------------------------------------------------
// Procedure: setDrawPointSize

void EncounterViewer::setDrawPointSize(string mod)
{
  if((mod == "smaller") && (m_draw_pointsize > 1))
    m_draw_pointsize--;
  else if((mod == "bigger") && (m_draw_pointsize < 20))
    m_draw_pointsize++;
}

//-------------------------------------------------------------
// Procedure: setTime()

void EncounterViewer::setTime(double time)
{
  m_curr_time = time;
}


//-------------------------------------------------------------
// Procedure: getTotalEncounters()

unsigned int EncounterViewer::getTotalEncounters() const
{
  unsigned int total_encounters = m_encounter_plot.size();
  return(total_encounters);
}
  
//-------------------------------------------------------------
// Procedure: getCurrTime()

double EncounterViewer::getCurrTime() const
{
  return(m_curr_time);
}






