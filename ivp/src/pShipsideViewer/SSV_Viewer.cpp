/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_Viewer.cpp                                       */
/*    DATE: September 11th, 2007                                 */
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

#include <math.h>
#include <tiffio.h>
#include "SSV_Viewer.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "ColorParse.h"
#include "XYBuildUtils.h"

using namespace std;

SSV_Viewer::SSV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_radial_size        = 100;
  m_centric_view       = true;
  m_draw_bearing_lines = true;
  m_draw_radial        = true;

  setParam("bearing_color", "orange");
  setParam("radial_color",  "red");
}

//-------------------------------------------------------------
// Procedure: draw()
//      Note: A mutex is put around all the drawing calls since it
//            is accessing information that is perhaps being 
//            altered by another thread.

void SSV_Viewer::draw()
{
  MarineViewer::draw();

  mutexLock();
  drawPolygons();
  drawGrids();
  drawSegLists();
  drawCircles();
  MarineViewer::drawPoints();
  drawStationCircles();
  if(m_draw_radial)
    drawRadials();
  if(m_draw_bearing_lines)
    drawBearingLine();

  if(m_vehiset.isViewable("vehicles")) {
    vector<string> svector = m_vehiset.getVehiNames();
    int vsize = svector.size();
    for(int i=0; i<vsize; i++) {
      string vehiname = svector[i];
      bool   isactive = (vehiname == m_vehiset.getActiveVehicle());
      string vehibody;
      bool   handled  = m_vehiset.getStringInfo(vehiname, "body", vehibody);
      
      // Perhaps draw the history points for each vehicle.
      if(m_vehiset.isViewable("trails")) {
	CPList point_list = m_vehiset.getVehiHist(vehiname);
	drawPoints(point_list);
      }
      
      // Next draw the vehicle shapes. If the vehicle index is the 
      // one "active", draw it in a different color.
      if(handled)
	drawVehicle(vehiname, isactive, vehibody);
    }
  }
  mutexUnLock();
  glFlush();
}

//-------------------------------------------------------------
// Procedure: handle

int SSV_Viewer::handle(int event)
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
    break;
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: setParam
//      Note: A mutex is used since the member variables being set
//            are perhaps being altered by another thread.
//      Note: The parent class has its own mutex protection for
//            its setParam implementation.

bool SSV_Viewer::setParam(string param, string value)
{
  // Parent class has its own mutex protection - none needed here.
  if(MarineViewer::setParam(param, value))
    return(true);
  
  param = tolower(stripBlankEnds(param));
  value = stripBlankEnds(value);
  
  bool handled = false;
  bool center_needs_adjusting = false;

  mutexLock();
  if(param == "center_view") {
    center_needs_adjusting = true;
    m_centric_view = true;
    handled = true;
  }
  else if(param == "station_circle")
    handled = addStationCircle(value);
  else if(param == "rstation_circle")
    handled = addStationCircle(value);
  else if(param == "bearing_lines")
    handled = setBooleanOnString(m_draw_bearing_lines, value);
  else if(param == "bearing_color") {
    handled = isColor(value);
    if(handled)
      m_bearing_color = colorParse(value);  
  }
  else if(param == "radial_color") {
    handled = isColor(value);
    if(handled)
      m_radial_color = colorParse(value);  
  }
  else if(param == "ownship_name") {
    m_ownship_name = toupper(value);
    handled = true;
  }
  else if(param == "op_vertex") 
    handled = m_op_area.addVertex(value, m_geodesy);
  else if(param == "draw_radial")
    handled = setBooleanOnString(m_draw_radial, value);
  else
    handled = m_vehiset.setParam(param, value);

  mutexUnLock();

  if(center_needs_adjusting)
    setWeightedCenterView();

  return(handled);
}

//-------------------------------------------------------------
// Procedure: setParam
//      Note: A mutex is used since the member variables being set
//            are perhaps being altered or read by another thread.
//      Note: The parent class has its own mutex protection for
//            its setParam implementation.

bool SSV_Viewer::setParam(string param, double value)
{
  param = tolower(stripBlankEnds(param));
  
  // Intercept and disable the centric mode if user pans
  if((param == "pan_x") || (param == "pan_y")) {
    mutexLock();
    m_centric_view = false;
    mutexUnLock();
  }

  // Parent class has its own mutex protection - none needed here.
  bool handled = MarineViewer::setParam(param, value);

  if(!handled) {
    mutexLock();
    if(param == "radial_size") {
      m_radial_size = (int)(value);
      if(m_radial_size < 0)
	m_radial_size = 0;
      if(m_radial_size > 2000)
       m_radial_size = 2000;
      handled = true;
    }
    else if(param == "radial_increment") {
      int new_size = m_radial_size + ((int)(value));
      if((new_size >= 0) && (new_size <= 2000))
	m_radial_size = new_size;
      handled = true;
    }
    else 
      handled = m_vehiset.setParam(param, value);
    mutexUnLock();
  }
  
  return(handled);
}

// ----------------------------------------------------------
// Procedure: getStringInfo
//      Note: A mutex is used since the info being accessed here
//            is perhaps being altered by another thread.

string SSV_Viewer::getStringInfo(const string& info_type, int precision)
{
  mutexLock();
  string result = "error";

  if(info_type == "radial_size")
    result = doubleToString(m_radial_size, precision);
  else if(info_type == "left_click_info")
    result = m_left_click;
  else if(info_type == "right_click_info")
    result = m_right_click;
  else if(info_type == "right_click_rp_info")
    result = m_right_click_rp;
  else if(info_type == "absolute_bearing")
    result = doubleToString(getRelativeInfo("absolute_bearing"),precision);
  else if(info_type == "relative_bearing")
    result = doubleToString(getRelativeInfo("relatvie_bearing"),precision);
  else if(info_type == "relative_range")
    result = doubleToString(getRelativeInfo("relatvie_range"),precision);
  else {
    string sresult;
    bool   shandled = m_vehiset.getStringInfo("active", info_type, sresult);
    if(shandled) {
      result = sresult;
    }
    else {
      double dresult;
      bool   dhandled = m_vehiset.getDoubleInfo("active", info_type, dresult);
      if(dhandled)
	result = doubleToString(dresult, precision);
    }
  }
  
  mutexUnLock();
  return(result);
}
  

// ----------------------------------------------------------
// Procedure: getRelativeInfo
//   Purpose: Return info regarding the relationship between the special
//            ownship vehicle, and the currently "active" vehicle. 
// InfoTypes: (a) bearing (absolute)
//            (b) relative bearing
//            (c) range between vehicles
// Notes: No mutex is used here despite its accessing of data structures
//        written to by other threads. This is because this is a 
//        PRIVATE class function called only by a function which 
//        is using its own mutex.

double SSV_Viewer::getRelativeInfo(const string& info_type)
{
  ObjectPose pose_vh = m_vehiset.getObjectPose("active");
  ObjectPose pose_os = m_vehiset.getObjectPose(m_ownship_name);

  bool os_is_active = (m_ownship_name == m_vehiset.getActiveVehicle());

  if(!pose_vh.isValid() || !pose_os.isValid() || os_is_active)
    return(-1);
  // The value -1 is not returned on any conceivable cases below so
  // we use it as a way of indicating an error condition.


  double os_x = pose_os.getX();
  double os_y = pose_os.getY();
  double vh_x = pose_vh.getX();
  double vh_y = pose_vh.getY();

  double bearing = relAng(os_x, os_y, vh_x, vh_y);

  string info_type_lower = tolower(info_type);
  if(info_type_lower == "absolute_bearing")
    return(angle360(bearing));
  
  double rbearing = bearing - pose_os.getTheta();
  if(info_type_lower == "relative_bearing")
    return(angle360(rbearing));
  else if(info_type_lower == "relative_range")
    return(hypot((os_x - vh_x), (os_y - vh_y)));
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: drawVehicle(ObjectPose)
// Notes: No mutex is used here despite its accessing of data structures
//        written to by other threads. This is because this is a 
//        PRIVATE class function called only by a function which 
//        is using its own mutex.


void SSV_Viewer::drawVehicle(string vname, bool active, string vehibody)
{
  ObjectPose opose = m_vehiset.getObjectPose(vname);
  if(!opose.isValid())
    return;

  // If there has been no explicit mapping of color to the given vehicle
  // name then the "inactive_vehicle_color" will be returned below.
  vector<double> vehi_color;
  if(active)
    vehi_color = m_vehiset.getColor("active_vehicle_color");
  else
    vehi_color = m_vehiset.getColor(vname);
  
  vector<double> name_color = m_vehiset.getColor("vehicle_name_color");

  double shape_scale = m_vehiset.getDoubleInfo("active", "vehicle_shape_scale");

  bool vname_draw = m_vehiset.isViewable("vehicle_names");

  m_vehiset.getDoubleInfo("active", "vehicle_shape_scale", shape_scale);

  drawCommonVehicle(vname, opose, vehi_color, name_color, vehibody, 
		    shape_scale, vname_draw, 1);
}

//-------------------------------------------------------------
// Procedure: drawPoints
// Notes: No mutex is used here despite its accessing of data structures
//        written to by other threads. This is because this is a 
//        PRIVATE class function called only by a function which 
//        is using its own mutex.


void SSV_Viewer::drawPoints(CPList &cps)
{
  if(!m_vehiset.isViewable("trails"))
    return;

  vector<double> xvect;
  vector<double> yvect;

  int trails_gap = (int)(m_vehiset.getDoubleInfo("active", "trails_gap"));
  if(trails_gap < 1)
    trails_gap = 1;

  list<ColoredPoint>::iterator p;
  int i=0;
  for(p=cps.begin(); p!=cps.end(); p++) {
    if((i % trails_gap) == 0) {
      if(p->isValid()) {
	xvect.push_back(p->m_x);
	yvect.push_back(p->m_y);
      }
    }
    i++;
  }

  vector<double> cvect = m_vehiset.getColor("trails_color");
  double point_size = m_vehiset.getDoubleInfo("active", "trails_point_size");
  bool connections  = m_vehiset.isViewable("trails_connect");

  drawPointList(xvect, yvect, point_size, cvect, connections);
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse
//      Note: A mutex is used since the info being accessed here
//            is perhaps being altered by another thread.

void SSV_Viewer::handleLeftMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  mutexLock();
  m_left_click =  "x=" + doubleToString(sx,1) + ",";
  m_left_click += "y=" + doubleToString(sy,1);
  mutexUnLock();
  
  // cout << "Left Mouse click at [" << m_left_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: handleRightMouse
//      Note: A mutex is used since the info being accessed here
//            is perhaps being altered by another thread.

void SSV_Viewer::handleRightMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  mutexLock();
  m_right_click =  "x=" + doubleToString(sx,1) + ",";
  m_right_click += "y=" + doubleToString(sy,1);
  mutexUnLock();

  ObjectPose opose = m_vehiset.getObjectPose(m_ownship_name);
  
  if(opose.isValid()) {
    double osx = opose.getX();
    double osy = opose.getY();
    double osh = opose.getTheta();
    
    double range = hypot((osx-sx),(osy-sy));
    double relang = relAng(osx, osy, sx, sy);

    relang = angle360(relang - osh);
    mutexLock();
    m_right_click_rp  = "range=" + doubleToString(range,1) + ",";
    m_right_click_rp += "bearing=" + doubleToString(relang) + ",";
    m_right_click_rp += "contact=" + m_ownship_name;
    mutexUnLock();
  
    //cout << "Right Mouse click at [" << m_right_click_rp << "] relpos." << endl;
    //cout << "Ownship-X: " << osx << "  Ownship-Y: " << osy << endl;
  }
  
  //cout << "Right Mouse click at [" << m_right_click << "] meters." << endl;
}


//-------------------------------------------------------------
// Procedure: drawRadials

void SSV_Viewer::drawRadials()
{
  // First determine the position of "ownship". If this isn't 
  // known we return right away.
  
  ObjectPose opose = m_vehiset.getObjectPose(m_ownship_name);
  if(!opose.isValid())
    return;

  double px = opose.getX();
  double py = opose.getY();
  int    psize = 60;

  bool   dashed = false;

  string poly_str = "radial:";
  poly_str += doubleToString(px,2) + ",";
  poly_str += doubleToString(py,2) + ",";
  poly_str += doubleToString(m_radial_size,2) + ",";
  poly_str += intToString(psize);
  
  XYPolygon poly = stringToPoly(poly_str);

  double *points = new double[2*psize];
  
  int pindex = 0;
  for(int i=0; i<psize; i++) {
    points[pindex]   = poly.get_vx(i);
    points[pindex+1] = poly.get_vy(i);

    points[pindex]   *=  m_back_img.get_pix_per_mtr();
    points[pindex+1] *=  m_back_img.get_pix_per_mtr();

    pindex += 2;
  }

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

  // Draw the edges 
  glLineWidth(1.0);
  if(dashed) {
    glEnable(GL_LINE_STIPPLE);
    GLushort pattern = 0x5555;
    GLint factor = 5;
    glLineStipple(factor, pattern);
  }

  vector<double> cvect = m_radial_color;
  glColor3f(cvect[0], cvect[1], cvect[2]);

  glBegin(GL_LINE_LOOP);
  for(int j=0; j<psize*2; j=j+2) 
    glVertex2f(points[j], points[j+1]);

  glEnd();
  if(dashed)
    glDisable(GL_LINE_STIPPLE);

  delete [] points;
  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: drawBearingLine

void SSV_Viewer::drawBearingLine()
{
  ObjectPose pose_vh = m_vehiset.getObjectPose("active");
  ObjectPose pose_os = m_vehiset.getObjectPose(m_ownship_name);

  if(!pose_vh.isValid() || !pose_os.isValid())
    return;

  double pt_0 = pose_os.getX() * m_back_img.get_pix_per_mtr();
  double pt_1 = pose_os.getY() * m_back_img.get_pix_per_mtr();
  double pt_2 = pose_vh.getX() * m_back_img.get_pix_per_mtr();
  double pt_3 = pose_vh.getY() * m_back_img.get_pix_per_mtr();
  
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

  vector<double> cvect = m_bearing_color;

  // Draw the edges 
  glLineWidth(1.0);
  glColor3f(cvect[0], cvect[1], cvect[2]);
  glBegin(GL_LINE_STRIP);

  glBegin(GL_LINE_STRIP);
  glVertex2f(pt_0, pt_1);
  glVertex2f(pt_2, pt_3);
  glEnd();

  glFlush();
  glPopMatrix();
}

//-------------------------------------------------------------
// Procedure: addStationCircle
// Notes: No mutex is used here despite its accessing of data structures
//        written to by other threads. This is because this is a 
//        PRIVATE class function called only by a function which 
//        is using its own mutex.

bool SSV_Viewer::addStationCircle(const string& str)
{
  XYCircle new_circ;
  
  bool ok = new_circ.initialize(str);
  if(!ok)
    return(false);

  string new_label = new_circ.getLabel();
  
  // Station Keeping Circles must have a label or else disregard
  if(new_label == "")
    return(false);
  
  bool prior_existed = false;
  
  int vsize = m_station_circ.size();
  for(int i=0; i<vsize; i++) {
    if(m_station_circ[i].getLabel() == new_label) {
      m_station_circ[i] = new_circ;
      prior_existed = true;
    }
  }
  if(!prior_existed)
    m_station_circ.push_back(new_circ);

  return(true);
}

//-------------------------------------------------------------
// Procedure: drawStationCircles

void SSV_Viewer::drawStationCircles()
{
  vector<double> edge_c = colorParse("blue");
  vector<double> fill_c = colorParse("dark_blue");
  vector<double> vert_c = colorParse("blue");
  vector<double> labl_c = colorParse("white");
  
  int vsize = m_station_circ.size();
  for(int i=0; i<vsize; i++)
    drawCircle(m_station_circ[i], 16, true, 
	       edge_c, fill_c, vert_c, labl_c);
}

//-------------------------------------------------------------
// Procedure: setWeightedCenterView()
//      Note: A mutex is used since the member variables being set
//            are perhaps being altered by another thread.

void SSV_Viewer::setWeightedCenterView()
{
  if(!m_centric_view)
    return;

  double avg_pos_x, avg_pos_y;
  bool ok = m_vehiset.getWeightedCenter(avg_pos_x, avg_pos_y);
  if(!ok)
    return;

  // First determine how much we're off in terms of meters
  double delta_x = avg_pos_x - m_back_img.get_x_at_img_ctr();
  double delta_y = avg_pos_y - m_back_img.get_y_at_img_ctr();
  
  // Next determine how much in terms of pixels
  double pix_per_mtr = m_back_img.get_pix_per_mtr();
  double x_pixels = pix_per_mtr * delta_x;
  double y_pixels = pix_per_mtr * delta_y;
  
  mutexLock();
  m_vshift_x = -x_pixels;
  m_vshift_y = -y_pixels;
  mutexUnLock();
}

