/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Viewer.cpp                                       */
/*    DATE: May 12th 2006                                       */
/****************************************************************/

#include "FV_Viewer.h"
#include "ColorParse.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

FV_Viewer::FV_Viewer(int x, int y, int wid, int hgt, const char *l)
  : Common_IPFViewer(x, y, wid, hgt, l)
{
  m_model = 0;

  m_clear_color.setColor("macbeige");
}

//-------------------------------------------------------------
// Procedure: resetQuadSet

void FV_Viewer::resetQuadSet()
{
  if(!m_model)
    return;
  
  m_quadset = m_model->getQuadSet();
}


//-------------------------------------------------------------
// Procedure: draw

void FV_Viewer::draw()
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
}
