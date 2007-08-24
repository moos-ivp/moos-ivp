/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TrajectorySet.cpp                                    */
/*    DATE: July 16th, 2007                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include "TrajectorySet.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

void TrajectorySet::addTrajectory(double sx, double sy, double sv,
				  double sh, double dx, double dy)
{
  m_start_x.push_back(sx);
  m_start_y.push_back(sy);
  m_start_spd.push_back(sv);
  m_start_hdg.push_back(sh);
  m_dest_x.push_back(dx);
  m_dest_y.push_back(dy);
}

//-----------------------------------------------------------------
// Procedure: getStartX
//            getStartY
//            getStartSPD
//            getStartHDG
//            getDestX
//            getDestY

double TrajectorySet::getStartX(int ix)
{
  if((ix >= 0) && (ix < m_start_x.size()))
    return(m_start_x[ix]);
}

double TrajectorySet::getStartY(int ix)
{
  if((ix >= 0) && (ix < m_start_y.size()))
    return(m_start_y[ix]);
}

double TrajectorySet::getStartSPD(int ix)
{
  if((ix >= 0) && (ix < m_start_spd.size()))
    return(m_start_spd[ix]);
}

double TrajectorySet::getStartHDG(int ix)
{
  if((ix >= 0) && (ix < m_start_hdg.size()))
    return(m_start_hdg[ix]);
}

double TrajectorySet::getDestX(int ix)
{
  if((ix >= 0) && (ix < m_dest_x.size()))
    return(m_dest_x[ix]);
}

double TrajectorySet::getDestY(int ix)
{
  if((ix >= 0) && (ix < m_dest_y.size()))
    return(m_dest_y[ix]);
}

//-----------------------------------------------------------------
// Procedure: asString

string TrajectorySet::asString(string format)
{
  int    vsize = m_start_x.size();
  string vsz = intToString(vsize);
  string str; 

  if(format == "concise") {
    for(int i=0; i<vsize; i++) {
      string sx  = doubleToString(m_start_x[i],1);
      string sy  = doubleToString(m_start_y[i],1);
      string spd = doubleToString(m_start_spd[i],1);
      string hdg = doubleToString(m_start_hdg[i],1);
      string dx  = doubleToString(m_dest_x[i],1);
      string dy  = doubleToString(m_dest_y[i],1);
      str += " # ";
      str += padString(sx, 6, true)  + ",";
      str += padString(sy, 6, true)  + ",";
      str += padString(spd, 4, true) + ",";
      str += padString(hdg, 5, true) + ",";
      str += padString(dx, 6, true)  + ",";
      str += padString(dy, 6, true);
    }
    return(str);
  }
  
  for(int i=0; i<vsize; i++) {
    str += "[" + intToString(i+1) + " of " + vsz + "]:";
    str += "sx="  + doubleToString(m_start_x[i],1) + ",";
    str += "sy="  + doubleToString(m_start_y[i],1) + ",";
    str += "spd=" + doubleToString(m_start_spd[i],1) + ",";
    str += "hdg=" + doubleToString(m_start_hdg[i],1) + ",";
    str += "dx="  + doubleToString(m_dest_x[i],1) + ",";
    str += "dy="  + doubleToString(m_dest_y[i],1);
  }
  return(str);
}

