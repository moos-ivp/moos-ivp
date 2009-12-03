/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: CircularUtils.cpp                                    */
/*    DATE: Jan 1st, 2007 Monday afternoon at Brueggers          */
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
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "CircularUtils.h"

#ifndef M_PI
#define M_PI 3.1415926
#endif


using namespace std;

//---------------------------------------------------------------
// Procedure: arcFlight
//   Purpose: 

bool arcFlight(double x1, double y1, double heading, double radius,
	       double dist, bool port, double& rx, double& ry, 
	       double& rheading)
{
  if((dist < 0) || (radius < 0))
    return(false);

  // Calculate the pivot point by calculating the angle from 
  // the start point to the pivot point and then projecting.
  double ang_ctr_to_start;
  double ang_ctr_to_finish;
  if(port) {
    ang_ctr_to_start = heading + 90;
    if(ang_ctr_to_start >= 360)
      ang_ctr_to_start -= 360;
  }
  else {
    ang_ctr_to_start = heading - 90;
    if(ang_ctr_to_start < 0)
      ang_ctr_to_start += 360;
  }

  double circumf   = M_PI * radius * 2;
  double arc_pct   = dist / circumf;
  double delta_hdg = arc_pct * 360;

  // Calculate the new-heading for return and for calculating
  // the new or return x,y position.
  if(delta_hdg >= 360)
    delta_hdg = angle360(delta_hdg);

  if(port)
    delta_hdg = -delta_hdg + 360;

  rheading = heading + delta_hdg; 
  ang_ctr_to_finish = ang_ctr_to_start + delta_hdg;
  if(rheading >= 360)
    rheading -= 360;
  if(ang_ctr_to_finish >= 360)
    ang_ctr_to_finish -= 360;

  // Now calculate the start x,y position
  double mod_ang_cts = -ang_ctr_to_start + 90;
  double radians     = (mod_ang_cts/180.0) * M_PI;
  double ctr_x       = x1 - (cos(radians) * radius);
  double ctr_y       = y1 - (sin(radians) * radius);

  // Now calculate the new x,y position
  double mod_ang_ctf = -ang_ctr_to_finish + 90;
  double new_radians = (mod_ang_ctf/180.0) * M_PI;
  rx                 = ctr_x + (cos(new_radians) * radius);
  ry                 = ctr_y + (sin(new_radians) * radius);

  return(true);
}


//---------------------------------------------------------------
// Procedure: arcFlight
//   Purpose: 

XYArc arcFlight(double x1, double y1, double heading, double radius,
		double dist, bool port)
{
  XYArc arc;
  if((dist < 0) || (radius < 0))
    return(arc);

  // Calculate the pivot point by calculating the angle from 
  // the start point to the pivot point and then projecting.
  double ang_ctr_to_start;
  double ang_ctr_to_finish;
  if(port) {
    ang_ctr_to_start = heading + 90;
    if(ang_ctr_to_start >= 360)
      ang_ctr_to_start -= 360;
  }
  else {
    ang_ctr_to_start = heading - 90;
    if(ang_ctr_to_start < 0)
      ang_ctr_to_start += 360;
  }

  double circumf   = M_PI * radius * 2;
  double arc_pct   = dist / circumf;
  double delta_hdg = arc_pct * 360;

  // Calculate the new-heading for return and for calculating
  // the new or return x,y position.
  if(delta_hdg >= 360)
    delta_hdg = angle360(delta_hdg);

  if(port)
    delta_hdg = -delta_hdg + 360;

  ang_ctr_to_finish = ang_ctr_to_start + delta_hdg;
  if(ang_ctr_to_finish >= 360)
    ang_ctr_to_finish -= 360;

  // Now calculate the start x,y position
  double mod_ang_cts = -ang_ctr_to_start + 90;
  double radians     = (mod_ang_cts/180.0) * M_PI;
  double ctr_x       = x1 - (cos(radians) * radius);
  double ctr_y       = y1 - (sin(radians) * radius);

  // Now calculate the new x,y position
  double mod_ang_ctf = -ang_ctr_to_finish + 90;
  double new_radians = (mod_ang_ctf/180.0) * M_PI;
  double rx          = ctr_x + (cos(new_radians) * radius);
  double ry          = ctr_y + (sin(new_radians) * radius);

  if(port)
    arc.set(ctr_x,ctr_y,radius,ang_ctr_to_start,
	    ang_ctr_to_finish,x1,y1,rx,ry);
  else
    arc.set(ctr_x,ctr_y,radius,ang_ctr_to_finish,
	    ang_ctr_to_start,rx,ry,x1,y1);

  return(arc);
}


