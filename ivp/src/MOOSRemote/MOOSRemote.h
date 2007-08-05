/*************************************************************************
 * MOOSRemote.h - 
 *************************************************************************
 * (c) 2003 Andrew Patrikalakis <anrp@cml3.mit.edu>                      *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the Free Software           *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *************************************************************************/


#ifndef __MOOSRemote_h__
#define __MOOSRemote_h__

#include "MOOSLib.h"
#include <curses.h>
#include <string>
#include <vector>

using namespace std;

#ifndef CLAMP
#define CLAMP(x,h,l) (x>h?h:x<l?l:x)
#endif

#ifndef ABS
#define ABS(x) (x>0?x:-x)
#endif

class CMOOSRemote {
private:
  // status stuff
  bool is_auto;
  double my_thrust, my_rudder, my_elevator;
  
  // position stuff
  double robotx, roboty, robott;
  string type;
  
  // logger stuff
  long logLastHit;
  bool is_log_on;
  string loggerName, loggerFileName;
  time_t openTime;
  
  // info window stuff
  vector<string> info_messages;
  int infow_scroll;
  
  void append_info_message(string);
  void addim(const char *, ...);
  
  // curses stuff:
  WINDOW *statw, *posw, *logw, *infow;
  bool has_statw_changed;
  bool has_posw_changed;
  bool has_logw_changed;
  bool has_infow_changed;
  
  void redraw_mainw(void);
  void redraw_statw(void);
  void redraw_posw(void);
  void redraw_logw(void);
  void redraw_infow(void);
  void redraw_all(void);
  
  int askyn(string);
  void help(void);
  void termtitle(string);
  void windtitle(WINDOW *, string);
  void msgbox(string);
  void getinput(string, string &);
  void save_message_log(void);
  
  void setup_curses(void);
  void create_windows(void);
  void raise_stop(void);
  
  int main_loop(void);
  
  // MOOS stuff
  CMOOSCommClient m_Comms;
  string mServer; int mPort;
  
  static bool OnConnectCallBack(void *);
  static bool OnDisconnectCallBack(void *);
  
  pthread_t comms_thread;
  static void *comms_main(void *);
  
  // time stuff
  string isodate, isotime;
  void updtime(void);
  
 public:
  CMOOSRemote(string server, int port);
  ~CMOOSRemote();
  
  void Run();
};

#endif /* __MOOSRemote_h__ */
