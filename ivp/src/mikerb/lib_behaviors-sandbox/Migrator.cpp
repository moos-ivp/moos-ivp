/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Migrator.cpp                                         */
/*    DATE: May 10th, 2007                                       */
/*****************************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <math.h>
#include "Migrator.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

Migrator::Migrator()
{
  m_curr_ix         = 0;
  m_reverse         = false;
  m_complete        = true;
}

//-----------------------------------------------------------
// Procedure: setSegList
//      Note: We store the value of m_reverse so the desired
//            effect is achieved regardless of whether the points
//            are set first, or the reverse-flag is set first.

void Migrator::setSegList(const XYSegList& g_seglist)
{
  // This setting should reset the curr_ix to zero should this
  // function call come in-progress.
  m_curr_ix  = 0;
  m_complete = false;

  m_seglist_raw = g_seglist;
  m_seglist     = g_seglist;

  if(m_reverse)
    m_seglist.reverse();
}

//-----------------------------------------------------------
// Procedure: setSegList

void Migrator::setSegList(double g_x, double g_y)
{
  XYSegList new_seglist;
  new_seglist.add_vertex(g_x, g_y);
  setSegList(new_seglist);
}

//-----------------------------------------------------------
// Procedure: setInitialConditions

void Migrator::setInitialConditions(double g_x, double g_y, 
				    double g_time)
{
  m_start_x = g_x;
  m_start_y = g_y;
  m_start_time = g_time;
}

//-----------------------------------------------------------
// Procedure: setReverse
//      Note: We store the value of m_reverse so the desired
//            effect is achieved regardless of whether the points
//            are set first, or the reverse-flag is set first.

void Migrator::setReverse(bool g_val)
{
  // If this is not a change in state, return immediately.
  if(m_reverse == g_val)
    return;
  m_reverse = g_val;

  if(g_val == true) {
    m_seglist = m_seglist_raw;
    m_seglist.reverse();
  }
  else
    m_seglist = m_seglist_raw;
  
  // This setting should reset the curr_ix to zero should this
  // function call come in-progress.
  m_curr_ix  = 0;
  m_complete = false;
}

//-----------------------------------------------------------
// Procedure: setCurrIndex
//   Purpose: Typically the current index starts at zero and
//            is incremented as it hits waypoints. But we also
//            may want to let a user set this directly.

void Migrator::setCurrIndex(int index)
{
  if((index < 0) || (index >= m_seglist.size()))
    return;
  
  m_curr_ix     = index;
}

//-----------------------------------------------------------
// Procedure: getPointX

double Migrator::getPointX(int i)
{
  if((i >= 0) && (i < m_seglist.size()))
    return(m_seglist.get_vx(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: getPointY

double Migrator::getPointY(int i)
{
  if((i >= 0) && (i < m_seglist.size()))
    return(m_seglist.get_vy(i));
  else
    return(0);
}

//-----------------------------------------------------------
// Procedure: updatePosition

void Migrator::updatePosition(double g_time)
{
  int segsize = m_seglist.size();
  if(segsize == 0)
    return;

  // calculate current distance based on start_time and speed
  double cdist = (g_time - m_start_time) * m_velocity;
  double ax    = m_start_x;
  double ay    = m_start_y;

  for(int i=0; i<segsize; i++) {
    double bx = m_seglist.get_vx(i);
    double by = m_seglist.get_vy(i);
    double dist_atob = hypot(bx-ax, by-ay);
    if(dist_atob > cdist) {
      double pct = cdist / dist_atob;
      if(bx == ax)
	m_curr_x = ax;
      else if(bx > ax)
	m_curr_x = (pct * (bx-ax)) + ax;
      else
	m_curr_x = ax - (pct * (ax-bx));

      if(by == ay)
	m_curr_y = ay;
      else if(by > ay)
	m_curr_y = (pct * (by-ay)) + ay;
      else
	m_curr_y = ay - (pct * (ay-by));
      
      return;
    }
    else {
      cdist -= dist_atob;
      ax = bx;
      ay = by;
    }
  }
  

}
