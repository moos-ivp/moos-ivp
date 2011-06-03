/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPFViewer.cpp                                        */
/*    DATE: Feb 13, 2007                                         */
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
#include "IPFViewer.h"
#include "FunctionEncoder.h"

using namespace std;

// ----------------------------------------------------------
// Constructor

IPFViewer::IPFViewer(CPA_Model* g_model, int g_x, int g_y, 
		     int g_width, int g_height, const char *g_l)
 : Common_IPFViewer(g_x, g_y, g_width, g_height, g_l)
{
  m_cpa_model = g_model;
  m_view_type = 0;

  m_clear_color.setColor("0.285,0.242,0.469");
  m_clear_color.setColor("white");
}

//-------------------------------------------------------------
// Procedure: draw()

void IPFViewer::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);

  Common_IPFViewer::drawIvPFunction();
  if(m_draw_frame)
    drawFrame();
  drawOwnPoint();
  glPopMatrix();
  
  glFlush();

  //GLenum err = glGetError();
  //if(err != GL_NO_ERROR)
  //  cout << "WARNING!!!!! GL ERROR DETECTED!!!!" << endl;
}

// ----------------------------------------------------------
// Procedure: setTypeIPF
//   Purpose: 

void IPFViewer::setTypeIPF(const string& str)
{
  if(str == "cpa")
    m_view_type = 0;
  else if(str == "wpt")
    m_view_type = 1;
  else if(str == "avd")
    m_view_type = 2;
  else if(str == "roc")
    m_view_type = 3;
  else if(str == "r16")
    m_view_type = 4;
}


// ----------------------------------------------------------
// Procedure: resetIPF
//   Purpose: 

void IPFViewer::resetIPF(int cn_index, int unif_units)
{
  if(unif_units < 1)
    return;

  IvPFunction *ipf = 0;
  if(m_view_type == 0)
    ipf = m_cpa_model->calc_cpa_ipf(cn_index, unif_units);
  else if(m_view_type == 1)
    ipf = m_cpa_model->calc_wpt_ipf(cn_index, unif_units);
  else if(m_view_type == 2)
    ipf = m_cpa_model->calc_avd_ipf(cn_index, unif_units);
  else if(m_view_type == 3)
    ipf = m_cpa_model->calc_roc_ipf(cn_index, unif_units);
  else if(m_view_type == 4)
    ipf = m_cpa_model->calc_r16_ipf(cn_index, unif_units);

  if(ipf) {
    m_quadset.applyIPF(ipf);
    m_quadset.normalize(0, 100);
    m_quadset.applyColorMap(m_color_map);
    delete(ipf);
  }
    
  redraw();
}



