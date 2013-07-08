/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IPFViewer.cpp                                        */
/*    DATE: Feb 13, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "IPFViewer.h"
#include "FunctionEncoder.h"

using namespace std;

// ----------------------------------------------------------
// Constructor

IPFViewer::IPFViewer(PairModel* g_model, int g_x, int g_y, 
		     int g_width, int g_height, const char *g_l)
 : Common_IPFViewer(g_x, g_y, g_width, g_height, g_l)
{
  m_pair_model = g_model;
  m_view_type  = 0;
  m_behavior   = 0;
  m_rad_extra  = 0.70;
  
  setParam("reset_view", "2");

  m_clear_color.setColor("0.285,0.242,0.469");
  m_clear_color.setColor("white");

  m_domain.addDomain("course", 0, 359, 360);
  m_domain.addDomain("speed",  0, 5, 51);

  m_bhv_factory_static.setDomain(m_domain);
  m_bhv_factory_dynamic.setDomain(m_domain);
  m_bhv_factory_dynamic.loadEnvVarDirectories("IVP_BEHAVIOR_DIRS");

  m_info_buffer = new InfoBuffer();
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
}

// ----------------------------------------------------------
// Procedure: resetIPF
//   Purpose: 

void IPFViewer::resetIPF(unsigned int unif_units, bool full_reset)
{
  resetIPF_COL(unif_units, full_reset);

  if(m_behavior) {
    double pwt_inner_dist = m_behavior->getDoubleInfo("pwt_inner_dist");
    double pwt_outer_dist = m_behavior->getDoubleInfo("pwt_outer_dist");
    m_pair_model->set_pwt_inner_dist(pwt_inner_dist);
    m_pair_model->set_pwt_outer_dist(pwt_outer_dist);
  }
}

// ----------------------------------------------------------
// Procedure: getInfo
//   Purpose: 

string IPFViewer::getInfo(string param)
{
  if(m_behavior)
    return(m_behavior->getInfo(param));
  else
    return("");
}


// ----------------------------------------------------------
// Procedure: resetIPF_AVD
//   Purpose: 

void IPFViewer::resetIPF_AVD(unsigned int unif_units)
{
  // Part 1: Sanity checks
  if(unif_units < 1)
    return;
  if(!m_info_buffer)
    return;

  // Part 2: Update the InfoBuffer to be used by the behavior
  m_info_buffer->setValue("NAV_X", m_pair_model->get_osx());
  m_info_buffer->setValue("NAV_Y", m_pair_model->get_osy());
  m_info_buffer->setValue("NAV_HEADING", m_pair_model->get_osh());
  m_info_buffer->setValue("NAV_SPEED",   m_pair_model->get_osv());

  m_info_buffer->setValue("CONTACT_NAV_X", m_pair_model->get_cnx());
  m_info_buffer->setValue("CONTACT_NAV_Y", m_pair_model->get_cny());
  m_info_buffer->setValue("CONTACT_NAV_HEADING", m_pair_model->get_cnh());
  m_info_buffer->setValue("CONTACT_NAV_SPEED",   m_pair_model->get_cnv());
  m_info_buffer->setValue("CONTACT_NAV_UTC",   0);

  // Part 4: Delete the previous behavior instance if there is one
  if(m_behavior) {
    delete(m_behavior);
    m_behavior = 0;
  }

  // Part 3: Create the behavior and generate and IvPFunction
  bool known = m_bhv_factory_static.isKnownBehavior("BHV_AvoidCollision");
  if(!known)
    return;
  m_behavior = m_bhv_factory_static.newBehavior("BHV_AvoidCollision");
  if(!m_behavior) {
    //    cout << "Failed to make a behavior...." << endl;
    return;
  }

  //  cout << "Successfully made behavior!!!!" << endl;

  m_behavior->setInfoBuffer(m_info_buffer);
  m_behavior->setParam("contact", "contact");
  IvPFunction *ipf = m_behavior->onRunState();

  if(ipf) {
    cout << "Successfully made and IvP Function" << endl;
    m_quadset.applyIPF(ipf);
    m_quadset.normalize(0, 100);
    m_quadset.applyColorMap(m_color_map);
    delete(ipf);
  }
  else
    cout << "Failed to make an IvP Function" << endl;

  delete(m_behavior);

  redraw();
}

// ----------------------------------------------------------
// Procedure: resetIPF_COL
//   Purpose: 

void IPFViewer::resetIPF_COL(unsigned int unif_units, bool full_reset)
{
  // Part 1: Sanity checks
  if(unif_units < 1)
    return;
  if(!m_info_buffer)
    return;

  cout << "resetIPF_COL ===================================== Begin() " << endl;

  // Part 2: Update the InfoBuffer to be used by the behavior
  m_info_buffer->setValue("NAV_X",       m_pair_model->get_osx());
  m_info_buffer->setValue("NAV_Y",       m_pair_model->get_osy());
  m_info_buffer->setValue("NAV_HEADING", m_pair_model->get_osh());
  m_info_buffer->setValue("NAV_SPEED",   m_pair_model->get_osv());

  m_info_buffer->setValue("CONTACT_NAV_X",       m_pair_model->get_cnx());
  m_info_buffer->setValue("CONTACT_NAV_Y",       m_pair_model->get_cny());
  m_info_buffer->setValue("CONTACT_NAV_HEADING", m_pair_model->get_cnh());
  m_info_buffer->setValue("CONTACT_NAV_SPEED",   m_pair_model->get_cnv());
  m_info_buffer->setValue("CONTACT_NAV_UTC",   0);

  string prev_avoid_mode, prev_avoid_submode;
  if(m_behavior && !full_reset) {
    prev_avoid_mode    = m_behavior->getInfo("avoid_mode");
    prev_avoid_submode = m_behavior->getInfo("avoid_submode");
  }
  cout << "prev_avoid_mode: " << prev_avoid_mode << endl;
  cout << "prev_avoid_submode: " << prev_avoid_submode << endl;

  // Delete the previous behavior instance if there is one
  if(m_behavior) {
    delete(m_behavior);
    m_behavior = 0;
  }

  // Part 3: Create the behavior and generate and IvPFunction
  bool known = m_bhv_factory_dynamic.isKnownBehavior("BHV_AvdColregs");
  if(!known)
    return;

  m_behavior = m_bhv_factory_dynamic.newBehavior("BHV_AvdColregs");
  if(!m_behavior) {
    cout << "Failed to make a behavior...." << endl;
    return;
  }

  cout << "Successfully made behavior!!!!" << endl;

  m_behavior->setInfoBuffer(m_info_buffer);
  m_behavior->setParam("avoid_mode", prev_avoid_mode);
  m_behavior->setParam("avoid_submode", prev_avoid_submode);
  m_behavior->setParam("contact", "contact");
  m_behavior->setParam("pwt_inner_dist", "25");
  m_behavior->setParam("pwt_outer_dist", "50");
  //m_behavior->setParam("build_info", "uniform_piece=discrete@course:1,speed:1");
  m_behavior->onSetParamComplete();
  IvPFunction *ipf = m_behavior->onRunState();

  if(ipf) {
    cout << "Successfully made and IvP Function" << endl;
    m_quadset.applyIPF(ipf);
    m_quadset.normalize(0, 100);
    m_quadset.applyColorMap(m_color_map);
    delete(ipf);
  }
  else {
    m_quadset = QuadSet();
    cout << "Failed to make an IvP Function" << endl;
  }

  redraw();
  cout << "resetIPF_COL ===================================== End() " << endl;
}


