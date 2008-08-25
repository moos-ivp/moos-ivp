/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PMV_Viewer.cpp                                       */
/*    DATE: Nov 11th 2004                                        */
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
#include "PMV_Viewer.h"
#include "MBUtils.h"
#include "ColorParse.h"

using namespace std;

PMV_Viewer::PMV_Viewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_centric_view = true;
}

//-------------------------------------------------------------
// Procedure: draw()

void PMV_Viewer::draw()
{
  cout << "requesting A"; mutexLock(); cout << "got A" << endl;
  MarineViewer::draw();

  drawPolygons();
  drawGrids();
  drawSegLists();
  drawCircles();
  MarineViewer::drawPoints();

  if(m_vehiset.isViewable("vehicles")) {
    vector<string> svector = m_vehiset.getVehiNames();
    int vsize = svector.size();
    for(int i=0; i<vsize; i++) {
      string vehiname = svector[i];
      bool   isactive = (vehiname == m_vehiset.getActiveVehicle());
      string vehibody = m_vehiset.getStringInfo(vehiname, "body");
      
      // Next draw the vehicle shapes. If the vehicle index is the 
      // one "active", draw it in a different color.
      drawVehicle(vehiname, isactive, vehibody);
      
      // Perhaps draw the history points for each vehicle.
      if(m_vehiset.isViewable("trails")) {
	CPList point_list = m_vehiset.getVehiHist(vehiname);
	drawPoints(point_list);
      }
    }
  }

  glFlush();
  cout << "releasing A "; mutexUnLock(); cout << "released A" << endl;
}

//-------------------------------------------------------------
// Procedure: handle

int PMV_Viewer::handle(int event)
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
//      Note: setBooleanOnString defined in MarineViewer.cpp

bool PMV_Viewer::setParam(string param, string value)
{
  cout << "requesting B"; mutexLock(); cout << "got B" << endl;
  if(MarineViewer::setCommonParam(param, value)) {
    cout << "releasing B "; mutexUnLock(); cout << "released B" << endl;
    return(true);
  }

  param = tolower(stripBlankEnds(param));
  value = stripBlankEnds(value);
  
  bool ok = false;

  if(param == "weighted_center_view") {
    setWeightedCenterView();
    m_centric_view = true;
  }
  else if(param == "ais_report") {
    ok = m_vehiset.setParam(param, value);
    if(ok && m_centric_view)
      setWeightedCenterView();
  }
  else
    ok = m_vehiset.setParam(param, value);

  cout << "releasing B "; mutexUnLock(); cout << "released B" << endl;
  return(ok);
}


//-------------------------------------------------------------
// Procedure: setParam

bool PMV_Viewer::setParam(string param, double value)
{
  cout << "requesting C"; mutexLock(); cout << "got C" << endl;

  // Intercept and disable the centric mode if user pans
  if((param == "pan_x") || (param == "pan_y"))
    m_centric_view = false;

  bool handled = MarineViewer::setCommonParam(param, value);
  if(!handled)
    handled = m_vehiset.setParam(param, value);

  cout << "releasing C "; mutexUnLock(); cout << "released C" << endl;
  return(handled);
}

// ----------------------------------------------------------
// Procedure: getStringInfo

string PMV_Viewer::getStringInfo(const string& info_type, int precision)
{
  cout << "requesting D"; mutexLock(); cout << "got D" << endl;
  string result = "error";

  if(info_type == "left_click_info")
    result = m_left_click;
  else if(info_type == "right_click_info")
    result = m_right_click;
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
  
  cout << "releasing D "; mutexUnLock(); cout << "released D" << endl;
  return(result);
}
  

//-------------------------------------------------------------
// Procedure: drawVehicle(ObjectPose)

void PMV_Viewer::drawVehicle(string vname, bool active, string vehibody)
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

  vector<double> vname_color = m_vehiset.getColor("vehicle_name_color");
  
  bool vname_draw = m_vehiset.isViewable("vehicle_names");
  
  double shape_scale = m_vehiset.getDoubleInfo("vehicle_shape_scale");
  
  drawCommonVehicle(vname, opose, vehi_color, vname_color, vehibody, 
		    shape_scale, vname_draw, 1);
}

//-------------------------------------------------------------
// Procedure: drawPoints

void PMV_Viewer::drawPoints(CPList &cps)
{
  if(!m_vehiset.isViewable("trails"))
    return;

  vector<double> xvect;
  vector<double> yvect;

  int trail_gap = (int)(m_vehiset.getDoubleInfo("trail_gap"));

  list<ColoredPoint>::iterator p;
  int i=0;
  for(p=cps.begin(); p!=cps.end(); p++) {
    if((i % trail_gap) == 0) {
      if(p->isValid()) {
	xvect.push_back(p->m_x);
	yvect.push_back(p->m_y);
      }
    }
    i++;
  }

  vector<double> cvect = m_vehiset.getColor("trail_color");
  double point_size = m_vehiset.getDoubleInfo("trail_point_size");

  drawPointList(xvect, yvect, point_size, cvect);
}

//-------------------------------------------------------------
// Procedure: handleLeftMouse

void PMV_Viewer::handleLeftMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);

  cout << "requesting E"; mutexLock(); cout << "got E" << endl;
  m_left_click =  "x=" + doubleToString(sx,1) + ",";
  m_left_click += "y=" + doubleToString(sy,1);
  cout << "releasing E "; mutexUnLock(); cout << "released E" << endl;

  cout << "Left Mouse click at [" << m_left_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: handleRightMouse

void PMV_Viewer::handleRightMouse(int vx, int vy)
{
  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, 1.0);
  double sy = snapToStep(my, 1.0);
  
  cout << "requesting F"; mutexLock(); cout << "got F" << endl;
  m_right_click =  "x=" + doubleToString(sx,1) + ",";
  m_right_click += "y=" + doubleToString(sy,1);
  cout << "releasing F "; mutexUnLock(); cout << "released F" << endl;

  cout << "Right Mouse click at [" << m_right_click << "] meters." << endl;
}

//-------------------------------------------------------------
// Procedure: setWeightedCenterView()

void PMV_Viewer::setWeightedCenterView()
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
  
  setParam("set_pan_x", -x_pixels);
  setParam("set_pan_y", -y_pixels);
}

