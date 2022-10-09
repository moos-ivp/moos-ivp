/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TurnGen180.cpp                                       */
/*    DATE: July 27th 2020                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "TurnGen180.h"
#include "XYCircle.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

TurnGen180::TurnGen180()
{
}

//-------------------------------------------------------------
// Procedure: generate()

void TurnGen180::generate()
{
  cout << "In TurnGen180::generate()" << endl;
  if(!valid())
    return;

  cout << "In TurnGen180::generate2()" << endl;
  m_points.clear();
  
  double cx = 0;
  double cy = 0;
  double port_angle = angle360(m_start_osh - 90);
  double star_angle = angle360(m_start_osh + 90);
  if(m_port_turn)
    projectPoint(port_angle, m_turn_radius, m_start_osx, m_start_osy, cx, cy);
  else
    projectPoint(star_angle, m_turn_radius, m_start_osx, m_start_osy, cx, cy);


  if(m_port_turn)
    cout << "Port Turn" << endl;
  else
    cout << "Starboard Turn" << endl;
  
  double px = 0;
  double py = 0;
  for(double delta=0; delta <= 180; delta+=m_ptgap ) {
    //cout << "delta:" << delta << endl;
    if(m_port_turn) {
      double angle = angle360(star_angle - delta);
      projectPoint(angle, m_turn_radius, cx, cy, px, py);
      XYPoint pt(px, py);
      //cout << "  px:" << doubleToString(px,1) << ", py:" << doubleToString(py) << endl;
      m_points.push_back(pt);
    }
    else {
      double angle = angle360(port_angle + delta);
      cout << "angle: " << angle << endl;
      projectPoint(angle, m_turn_radius, cx, cy, px, py);
      XYPoint pt(px, py);
      //cout << "    px:" << doubleToString(px,1) << ", py:" << doubleToString(py) << endl;
      m_points.push_back(pt);
    }
  }

}

