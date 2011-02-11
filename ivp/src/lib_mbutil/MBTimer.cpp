/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MBTimer.cpp                                          */
/*    DATE: Too long ago to remember (1996-1999)                 */
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
#include "MBTimer.h"

#ifdef _WIN32
#include <sys/timeb.h>
#endif

using namespace std;

//--------------------------------------------------------------
// Constructor

MBTimer::MBTimer(int t)
{
  //cout << "In MBTimer Constructor!!" << endl;
  total_wall_time       = 0;
  total_user_cpu_time   = 0;
  total_system_cpu_time = 0;
  turnedOn   = false;
  timedOut   = false;
  maxTime    = t;
  TCKS_PER_SEC = 100;
}


//--------------------------------------------------------------
// Procedure: Start
//   Purpose: Turn on the timer

void MBTimer::start()
{
  if(timedOut) return;
  if(turnedOn == true) {
    cout << "Timer Error: Tried to turn on running timer" << endl;    
    return;
  }
  turnedOn = true;
#ifndef _WIN32
  wall_start_time       = times(&b);
  user_cpu_start_time   = b.tms_utime;
  system_cpu_start_time = b.tms_stime;
#else
  
  struct _timeb timebuffer;
  _ftime( &timebuffer );
  wall_start_time = (clock_t)(TCKS_PER_SEC*(timebuffer.time+ ((double)timebuffer.millitm)/1000));
  user_cpu_start_time   = clock();
  system_cpu_start_time = clock();
#endif
}


//--------------------------------------------------------------
// Procedure: Stop
//   Purpose: Turn off the timer

void MBTimer::stop()
{
  if(timedOut) return;
  if(turnedOn == false) {
    cout << "Timer Error: Tried to turn off stopped timer" << endl;
    return;
  }
  turnedOn = false;
  
#ifndef _WIN32
  total_wall_time       += times(&b)   - wall_start_time;
  total_user_cpu_time   += b.tms_utime - user_cpu_start_time;
  total_system_cpu_time += b.tms_stime - system_cpu_start_time;
#else

  //PMN - note this XPC bit returns user and system time as teh same thing
  struct _timeb timebuffer;
  _ftime( &timebuffer );
  clock_t T = (clock_t)(TCKS_PER_SEC*(timebuffer.time+ ((double)timebuffer.millitm)/1000));

  total_wall_time       += T   - wall_start_time;
  total_user_cpu_time   += clock() - user_cpu_start_time;
  total_system_cpu_time += clock() - system_cpu_start_time;


#endif
}

//--------------------------------------------------------------
// Procedure: reset
//   Purpose: Reset all values to zero.

void MBTimer::reset(int t)
{
  if(timedOut) return;
  maxTime               = t;
  total_wall_time       = 0;
  total_user_cpu_time   = 0;
  total_system_cpu_time = 0;
  turnedOn = false;
}

//--------------------------------------------------------------
// Procedure: timeOutCheck()
//   Purpose: Checks if the total time so far is greater than the
//            maxTime allowed before timing out.
//   Returns: The resulting state of "timedOut".

bool MBTimer::timeOutCheck()
{
  clock_t t1 = (clock_t) maxTime;
  clock_t t2 = (clock_t) get_wall_time();

  if(maxTime && t2>t1) 
    timedOut = true;
  return(timedOut);
}


//--------------------------------------------------------------
// Procedure: get_wall_time
//   Purpose: return the wall time accumulated so far
//            Note: if the timer is not off, it will temporarily 
//            shut itself off to update the total time.

clock_t MBTimer::get_wall_time(int prec)
{
  clock_t return_value;
  if(turnedOn == false)
    return_value = total_wall_time; 
  else {
    this->stop();
    return_value = total_wall_time;
    this->start();
  }
  if((prec != 1) && (prec != 1000)) prec = 1;
  return( ((return_value * prec)) / (TCKS_PER_SEC));    
}


//--------------------------------------------------------------
// Procedure: get_user_cpu_time
//   Purpose: return the user cpu time accumulated so far
//            Note: if the timer is not off, it will temporarily 
//            shut itself off to update the total time.

clock_t MBTimer::get_user_cpu_time(int prec)
{
  clock_t return_value;
  if(turnedOn == false)
    return_value = total_user_cpu_time;
  else {
    this->stop();
    return_value = total_user_cpu_time;
    this->start();
  }
  if((prec != 1) && (prec != 1000)) prec = 1;
  return( ((return_value * prec)) / (TCKS_PER_SEC));    
}


//--------------------------------------------------------------
// Procedure: get_system_cpu_time
//   Purpose: return the system cpu time accumulated so far
//            Note: if the timer is not off, it will temporarily 
//            shut itself off to update the total time.

clock_t MBTimer::get_system_cpu_time(int prec)
{
  clock_t return_value;
  if(turnedOn == false)
    return_value = total_system_cpu_time;
  else {
    this->stop();
    return_value = total_system_cpu_time;
    this->start();
  }
  if((prec != 1) && (prec != 1000)) 
    prec = 1;
  return( ((return_value * prec)) / (TCKS_PER_SEC));    
}

//--------------------------------------------------------------
// Procedure: get_cpu_time
//   Purpose: return the cpu time accumulated so far. This simply
//            the sum of the system and user cpu times.
//            Note: if the timer is not off, it will temporarily 
//            shut itself off to update the total time.

clock_t MBTimer::get_cpu_time(int prec)
{
  clock_t return_value;
  if(turnedOn == false)
    return_value = total_system_cpu_time + total_user_cpu_time;
  else {
    this->stop();
    return_value = total_system_cpu_time + total_user_cpu_time;
    this->start();
  }
  if((prec != 1) && (prec != 1000)) prec = 1;
  return( ((return_value * prec)) / (TCKS_PER_SEC));    
} 









