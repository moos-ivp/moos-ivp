/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BNGViewer.cpp                                        */
/*    DATE: May 21, 2013                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <iostream>
#include <cmath>
#include "PAIR_Viewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

PAIR_Viewer::PAIR_Viewer(PairModel *model, int x, int y, int wid, int hgt,
		       const char *label) : MarineViewer(x, y, wid, hgt, label)
{
  m_pair_model = model;

  m_snap_val = 1.0;    // meters

  //m_vshift_x = -80;
  //m_vshift_y = -110;
  m_zoom     = 1.4;

  m_shape_scale = 0.7;

  setParam("tiff_view", "on");
  setParam("hash_viewable", "false");
  setParam("hash_shade", -1.0);
  setParam("hash_shade", 0.75);
  setParam("back_shade", -1.0);
  setParam("back_shade", 0.85);

  //m_geo_settings.setParam("hash_viewable", "true");

  m_cpack_os_vector.setColor("white");
  m_cpack_cn_vector.setColor("brown");

  m_draw_alert_circle   = true;
  m_draw_warning_circle = true;

  //  m_start_pan_x = -220;
  //  m_start_pan_y = 110;

  m_init_center_view = true;

  resetPan();
}


//-------------------------------------------------------------
// Procedure: handle()

int PAIR_Viewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handle_left_mouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handle_right_mouse(vx, vy);
    }
    return(1);
    break;  
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: draw()

void PAIR_Viewer::draw()
{
  MarineViewer::draw();
  //if(!m_init_center_view)
  initCenterView();  

  if(m_geo_settings.viewable("hash_viewable"))
    drawFastHash();

  vector<XYPolygon> polys = m_geoshapes.getPolygons();
  map<string, XYSegList> segls = m_geoshapes.getSegLists();

  const map<string, XYPoint>& points = m_geoshapes.getPoints();

  drawPolygons(polys);
  drawPoints(points);
  drawSegLists(segls);

  if(m_draw_alert_circle)
    drawAlertCircle();
  if(m_draw_warning_circle)
    drawWarningCircle();
  draw_vector("os");
  draw_vector("cn");
  drawOwnship();
  drawContact();
}

//-------------------------------------------------------------
// Procedure: drawOwnship()

void PAIR_Viewer::drawOwnship()
{
  NodeRecord  node_record;
  node_record.setX(m_pair_model->get_osx());
  node_record.setY(m_pair_model->get_osy());
  node_record.setHeading(m_pair_model->get_osh());
  node_record.setSpeed(m_pair_model->get_osv());
  node_record.setName("os");
  node_record.setLength(20 * m_shape_scale);
  node_record.setType("kayak");


  //BearingLine bng_line;
  ColorPack   body_color(1,0,0);
  ColorPack   vname_color(0.5,0.5,0.5);
  bool        vname_draw  = true;
  unsigned int outer_line = 1;

  drawCommonVehicle(node_record, body_color, vname_color, vname_draw, outer_line);
}

//-------------------------------------------------------------
// Procedure: drawContact()

void PAIR_Viewer::drawContact()
{
  NodeRecord  node_record;
  node_record.setX(m_pair_model->get_cnx());
  node_record.setY(m_pair_model->get_cny());
  node_record.setHeading(m_pair_model->get_cnh());
  node_record.setSpeed(m_pair_model->get_cnv());
  node_record.setName("cn");
  node_record.setLength(20 * m_shape_scale);
  node_record.setType("kayak");

  //BearingLine bng_line;
  ColorPack   body_color("yellow");
  ColorPack   vname_color(0.5,0.5,0.5);
  bool        vname_draw  = true;
  unsigned int outer_line = 1;

  drawCommonVehicle(node_record, body_color, vname_color, vname_draw, outer_line);
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse()

void PAIR_Viewer::handle_left_mouse(int vx, int vy)
{
  cout << "In handle_left_mouse----" << endl;
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);
  
  if(Fl::event_state(FL_CTRL)) {
    m_pair_model->set_cnx(sx);
    m_pair_model->set_cny(sy);
  }
  else if(Fl::event_state(FL_SHIFT)) {
    m_pair_model->set_osx(sx);
    m_pair_model->set_osy(sy);
  }
  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse()

void PAIR_Viewer::handle_right_mouse(int vx, int vy)
{
}

// ----------------------------------------------------------
// Procedure: draw_vector()

void PAIR_Viewer::draw_vector(string node)
{
  double vertex_size = 0.6;
  double red = 1;
  double grn = 1;
  double blu = 1;
  double node_x, node_y, node_h, node_v;
  if(node == "ownship") {
    node_x = m_pair_model->get_osx();
    node_y = m_pair_model->get_osy();
    node_h = m_pair_model->get_osh();
    node_v = m_pair_model->get_osv();
    red    = m_cpack_os_vector.red();
    grn    = m_cpack_os_vector.grn();
    blu    = m_cpack_os_vector.blu();
  }
  else if(node == "contact") {
    node_x = m_pair_model->get_cnx();
    node_y = m_pair_model->get_cny();
    node_h = m_pair_model->get_cnh();
    node_v = m_pair_model->get_cnv();
    red    = m_cpack_cn_vector.red();
    grn    = m_cpack_cn_vector.grn();
    blu    = m_cpack_cn_vector.blu();
  }
  else
    return;

  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w(), 0, h(), -1 ,1);

  double tx = meters2img('x', 0);
  double ty = meters2img('y', 0);
  double qx = img2view('x', tx);
  double qy = img2view('y', ty);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glTranslatef(qx, qy, 0);
  glScalef(m_zoom, m_zoom, m_zoom);

  
  // Begin draw the vector stem
  // - calculate the end of the vector stem
  double pt_newx, pt_newy;
  double distance = node_v * 60;
  projectPoint(node_h, distance, node_x, node_y, pt_newx, pt_newy);

  glPointSize(vertex_size * m_zoom);
  glColor3f(red, grn, blu); 
  glLineWidth(1.0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(node_x, node_y);
  glVertex2f(pt_newx, pt_newy);
  glEnd();
  glLineWidth(1.0);

  // Begin draw the arrow head
  // - calculate the arrowhead points
  double ptx1, pty1, ptx2, pty2, ptx3, pty3;
  projectPoint(node_h, 8, pt_newx, pt_newy, ptx1, pty1);
  projectPoint(node_h-90, 4, pt_newx, pt_newy, ptx2, pty2);
  projectPoint(node_h+90, 4, pt_newx, pt_newy, ptx3, pty3);

  // - draw the arrowhead points
  glEnable(GL_BLEND);
  glColor3f(red, grn, blu); 
  //glColor4f(vertex_r, vertex_g, vertex_b, 0.1);
  glBegin(GL_POLYGON);
  glVertex2f(ptx1, pty1);
  glVertex2f(ptx2, pty2);
  glVertex2f(ptx3, pty3);
  glEnd();
  glDisable(GL_BLEND);

  // Begin draw the vertex
  glEnable(GL_POINT_SMOOTH);
  glPointSize(3);
  glColor3f(0.3, 0, 0.7); 
  glBegin(GL_POINTS);
  glVertex2f(node_x, node_y);
  glEnd();

  // Wrap it up
  glFlush();
  glPopMatrix();
}

// ----------------------------------------------------------
// Procedure: resetPan()

void PAIR_Viewer::resetPan()
{
  m_vshift_x = m_start_pan_x;
  m_vshift_y = m_start_pan_y;  
}


//-------------------------------------------------------------
// Procedure: initCenterView()

void PAIR_Viewer::initCenterView()
{
  double pos_x = m_pair_model->get_avgx();
  double pos_y = m_pair_model->get_avgy();

  // First determine how much we're off in terms of meters
  double delta_x = pos_x - m_back_img.get_x_at_img_ctr();
  double delta_y = pos_y - m_back_img.get_y_at_img_ctr();
  
  // Next determine how much in terms of pixels
  double pix_per_mtr_x = m_back_img.get_pix_per_mtr_x();
  double pix_per_mtr_y = m_back_img.get_pix_per_mtr_y();

  double x_pixels = pix_per_mtr_x * delta_x;
  double y_pixels = pix_per_mtr_y * delta_y;
  
  m_vshift_x = -x_pixels;
  m_vshift_y = -y_pixels;
}


// ----------------------------------------------------------
// Procedure: drawAlertCircle()

void PAIR_Viewer::drawAlertCircle()
{
  string specs = "x=" + doubleToStringX(m_pair_model->get_cnx());
  specs += ",y=" + doubleToStringX(m_pair_model->get_cny());
  specs += ",radius=" + doubleToStringX(m_pair_model->get_alert_range());
  specs += ",pts=60";

  XYPolygon poly = stringRadial2Poly(specs);
  poly.set_label("rng");
  poly.set_label_color("off");
  poly.set_vertex_size(0);
  poly.set_edge_size(1);
  poly.set_edge_color("gray40");
  
  drawPolygon(poly);
}


// ----------------------------------------------------------
// Procedure: drawWarningCircle()

void PAIR_Viewer::drawWarningCircle()
{
  string specs = "x=" + doubleToStringX(m_pair_model->get_cnx());
  specs += ",y=" + doubleToStringX(m_pair_model->get_cny());

  double warning_rng = m_pair_model->get_warning_range();
  
  if(warning_rng < m_pair_model->get_alert_range())
    return;
  
  specs += ",radius=" + doubleToStringX(warning_rng);
  specs += ",pts=60";

  XYPolygon poly = stringRadial2Poly(specs);
  poly.set_label("warn");
  poly.set_label_color("off");
  poly.set_vertex_size(0);
  poly.set_edge_size(1);
  poly.set_edge_color("grey60");
  drawPolygon(poly);
}

