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

