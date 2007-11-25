/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MBTimer.h                                            */
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

#ifndef MBTIMER_HEADER
#define MBTIMER_HEADER

#ifndef _WIN32
	#include <sys/times.h>
#else
	#include <time.h>
	#define tms time_t
#endif

class MBTimer {
public:
  MBTimer(int=0);
  ~MBTimer() {};

  void start();
  void stop();
  void reset(int=0);
  bool timeOutCheck();
  bool isTimedOut() { return(timedOut);};
  int  getMaxTime() { return(maxTime); };

  clock_t get_wall_time(int prec=1);
  clock_t get_user_cpu_time(int prec=1);
  clock_t get_system_cpu_time(int prec=1);
  clock_t get_cpu_time(int prec=1);

  float   get_float_wall_time() 
  { return((float) get_wall_time(1000)/1000); };
  float   get_float_cpu_time() 
  { return((float) get_cpu_time(1000)/1000); };
  float   get_float_user_cpu_time() 
  { return((float) get_user_cpu_time(1000)/1000); };
  float   get_float_system_cpu_time() 
  { return((float) get_system_cpu_time(1000)/1000); };

private:

#ifndef _WIN32
  struct tms b;
  clock_t wall_start_time;
  clock_t user_cpu_start_time;
  clock_t system_cpu_start_time;

  clock_t  total_wall_time;
  clock_t  total_user_cpu_time;
  clock_t  total_system_cpu_time;
#else
  clock_t  wall_start_time;
  clock_t user_cpu_start_time;
  clock_t system_cpu_start_time;

  clock_t total_wall_time;
  clock_t  total_user_cpu_time;
  clock_t  total_system_cpu_time;

#endif

  
  int   TCKS_PER_SEC;
  bool  turnedOn;
  bool  timedOut;
  int   maxTime;
};

#endif









