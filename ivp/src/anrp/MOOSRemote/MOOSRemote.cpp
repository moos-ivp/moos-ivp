/*************************************************************************
 * MOOSRemote.cpp - 
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

#include <stdarg.h>

#include "MOOSRemote.h"

#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

CMOOSRemote::CMOOSRemote(string server, int port)
{
  is_auto = false;
  my_thrust = my_rudder = my_elevator = 0.0;
  
  robotx = roboty = robott = 0.0;
  type = "unknown";
  
  logLastHit = 0;
  is_log_on = false;
  loggerName = ""; loggerFileName = "";
  openTime = 0;
  
  info_messages.clear();
  infow_scroll = 0;
  has_infow_changed = true;
  has_statw_changed = true;
  has_logw_changed = true;
  has_posw_changed = true;
  
  m_Comms.SetOnConnectCallBack(&(CMOOSRemote::OnConnectCallBack), this);
  m_Comms.SetOnDisconnectCallBack(&(CMOOSRemote::OnDisconnectCallBack), this);
  
  mServer = server; mPort = port;
}

CMOOSRemote::~CMOOSRemote()
{
  ;
}

void CMOOSRemote::append_info_message(string s)
{
  info_messages.push_back(s);
  if(LINES-12 <= (signed)info_messages.size() - infow_scroll-1)
    infow_scroll++;
  
  has_infow_changed = true;
}

void CMOOSRemote::addim(const char *s, ...)
{
  va_list ap;
  char tmp[1024], tmp2[1024];
  
  va_start(ap, s);
  vsnprintf(tmp, 1024, s, ap);
  va_end(ap);

  updtime();
  snprintf(tmp2, 1024, "%s %s - %s", isodate.c_str(), isotime.c_str(), tmp);
  
  append_info_message(tmp2);
}

void CMOOSRemote::windtitle(WINDOW *wn, string title)
{
  mvwaddch(wn, 0, 2, '[');
  mvwaddstr(wn, 0, 3, title.c_str());
  mvwaddch(wn, 0, 3+title.size(), ']');
}

int CMOOSRemote::askyn(string p)
{
  WINDOW *wn;
  
  wn = newwin(3, 10+p.size(), (LINES&(~1))/2 - 2, (COLS&(~1))/2 - (5+p.size()/2));
  
  wborder(wn,0,0,0,0,0,0,0,0);
  windtitle(wn, " Question ");
  mvwaddstr(wn, 1, 2, p.c_str());
  mvwaddstr(wn, 1, 3+p.size(), "(Y/N)");
  wrefresh(wn);
  
  int sl;
  
  do {
    sl = getch();
  } while(sl != 'Y' && sl != 'N' && sl != 'y' && sl != 'n' && sl != 3 && sl != 13);
  
  delwin(wn);
  if(sl == 'Y' || sl == 'y' || sl == 13) return 1;
  else return 0;
}

void CMOOSRemote::help(void)
{
	int nh = 17, nw = 40;
	WINDOW *wn;

	wn = newwin(nh, nw, (LINES&(~1))/2 - nh/2, (COLS&(~1))/2 - nw/2);
	
	wborder(wn,0,0,0,0,0,0,0,0);
	windtitle(wn, " Help - hit any key to exit ");
	
	/*                    ------------------------------------ */
	mvwaddstr(wn,  1, 2, "q - Quit          ? - This help");
	mvwaddstr(wn,  2, 2, "a - Accelerate    z - Decelerate");
	mvwaddstr(wn,  3, 2, "n - Turn left     m - Turn right");
	mvwaddstr(wn,  4, 2, ", - Center rudder ` - Hit WD");
	mvwaddstr(wn,  5, 2, "<space> - Regain control and stop");
	mvwaddstr(wn,  6, 2, "A - Max thrust    Z - Min thrust");
	mvwaddstr(wn,  7, 2, "O - Toggle manual/auto control");
	mvwaddstr(wn,  8, 2, "G - Reset logger");
	mvwhline(wn, 9, 10, 0, 20);
	mvwaddstr(wn, 10, 2, "PgUp - Scroll up  PgDn - Scroll down");
	mvwaddstr(wn, 11, 2, "S - Save message log to file");
	mvwaddstr(wn, 12, 2, "C - Clear message information log");
	mvwhline(wn, 13, 10, 0, 20);
	mvwaddstr(wn, 14, 2, "L - start zLogger on a disconnected");
	mvwaddstr(wn, 15, 2, "    GNU screen session");
	wmove(wn, nh-1, nw-1);
	wrefresh(wn);
	getch();
	werase(wn);
	wrefresh(wn);
	delwin(wn);
}

void CMOOSRemote::termtitle(string title)
{
	if(getenv("DISPLAY"))
		printf("\033]0;%s\007", title.c_str());
}

void CMOOSRemote::msgbox(string msg)
{
	WINDOW *wn;
	int nh = 3, nw = msg.size()+4;

	wn = newwin(nh, nw, (LINES&(~1))/2 - nh/2, (COLS&(~1))/2 - nw/2);

	wborder(wn,0,0,0,0,0,0,0,0);
	windtitle(wn, " Message ");

	mvwaddstr(wn, 1, 2, msg.c_str());
	wmove(wn, nh-1, nw-1);
	wrefresh(wn);

	getch();

	delwin(wn);
}

void CMOOSRemote::getinput(string q, string &dest)
{
	char dst[61];
	WINDOW *wn;
	int nh = 5, nw = 60;

	wn = newwin(nh, nw, (LINES&(~1))/2 - nh/2, (COLS&(~1))/2 - nw/2);

	wborder(wn,0,0,0,0,0,0,0,0);
	windtitle(wn, " Input ");

	mvwaddstr(wn, 1, 2, q.c_str());
	curs_set(1);
	echo();
	wrefresh(wn);
	mvwgetnstr(wn, 3, 2, dst, 50);
	noecho();
	curs_set(0);
	
	delwin(wn);
	dest = string(dst);
}

void CMOOSRemote::setup_curses(void)
{
	initscr();
	keypad(stdscr, TRUE);
	nonl();
	raw();
	noecho();
	curs_set(0);
	termtitle("MOOSRemote");

	if(has_colors()) {
		start_color();

                init_pair(COLOR_BLACK,   COLOR_BLACK,   COLOR_BLACK);
                init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
                init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
                init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
                init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
	}
}

void CMOOSRemote::create_windows(void)
{
	statw = subwin(stdscr, 6, 24, 2, 3);
	posw  = subwin(stdscr, 6, 21, 2, 29);
	logw  = subwin(stdscr, 6, 25, 2, 52);
	infow = subwin(stdscr, LINES-10, COLS-6, 8, 3);
}

void CMOOSRemote::raise_stop(void)
{
	def_prog_mode();
	endwin();
	raise(SIGTSTP);
	refresh();
}

void CMOOSRemote::updtime(void)
{
	time_t lt; struct tm ltm; char tmp[100];

	time(&lt); localtime_r(&lt, &ltm);

	strftime(tmp, 30, "%Y-%m-%d", &ltm); isodate = tmp;
	strftime(tmp, 30, "%H:%M:%S", &ltm); isotime = tmp;
}

int CMOOSRemote::main_loop(void)
{
	int c;
	bool done = false;
	
	while(!done) {
		redraw_all();
		halfdelay(1);
		c = getch();
		raw();

		switch(c) {
		case '?':
			help();
			break;
		case KEY_NPAGE: // page down
			infow_scroll += LINES-12;
			if((unsigned int)infow_scroll > info_messages.size())
				infow_scroll = info_messages.size();
			has_infow_changed = true;
			break;
		case KEY_PPAGE: // page up
			infow_scroll -= LINES-12;
			if(infow_scroll < 0) infow_scroll = 0;
			has_infow_changed = true;
			break;
		case 'f':
			addim("fake info message rnd=%i", rand() % 16384);
			break;
		case 'C':
			info_messages.clear();
			has_infow_changed = true;
			break;
		case 'S':
			save_message_log();
			break;
		case 'G':
			if(askyn("Really reset the logger?")) {
				addim("Reset the logger");
				m_Comms.Notify("LOGGER_RESTART", "TRUE");
			}
			break;
		case 'q':
		case 'Q':
		case  3: // C-c
			if(askyn("Do you really want to quit?"))
				done = true;

			break;
		case 26:
			raise_stop();
			break;
		case KEY_UP:
		case 'a':
			if(!is_auto) {
				my_thrust += 2;
			}
			break;
		case KEY_DOWN:
		case 'z':
			if(!is_auto) {
				my_thrust -= 2;
			}
			break;
		case KEY_LEFT:
		case 'n':
			if(!is_auto) {
				my_rudder -= 2;
			}
			break;
		case KEY_RIGHT:
		case 'm':
			if(!is_auto) {
				my_rudder += 2;
			}
			break;
		case ',':
			if(!is_auto) {
				my_rudder = 0;
			}
			break;
		case '`':
			addim("Watchdog not implemented");
			break;
		case ' ':
			if(is_auto) addim("Stealing control");
			is_auto = false;
			my_rudder = my_elevator = my_thrust = 0.0;
			addim("Full stop!");
			break;
		case 'A':
			if(!is_auto && askyn("Really set maximum thrust?"))
				my_thrust = 100;
			break;
		case 'Z':
			if(!is_auto && askyn("Really set maximum thrust in reverse?"))
				my_thrust = -100;
			break;
		case 'O':
			has_statw_changed = true;
			if(is_auto) {
				if(askyn("Really retake manual control?")) {
					is_auto = false;
					addim("Retook manual control");
				}
			} else {
				if(askyn("Really relinquish manual control?")){
					is_auto = true;
					m_Comms.Notify("MOOS_MANUAL_OVERIDE", "TRUE");
					addim("Relinquished manual control");
				}
			}
			break;
		case 'L':
			system("screen -dmS zLogger ./zLogger");
			break;
		case ERR:
			// timeout on keypress; do nothing
			break;
		default:
			addim("Unknown key `%c\'", c);
			break;
		}

		if(!is_auto) {
			my_thrust = CLAMP(my_thrust, 100.0, -100.0);
			my_rudder = CLAMP(my_rudder, 100.0, -100.0);
			my_elevator = CLAMP(my_elevator, 100.0, -100.0);
			
			has_statw_changed = true;
			
			m_Comms.Notify("DESIRED_THRUST", my_thrust);
			m_Comms.Notify("DESIRED_RUDDER", my_rudder);
			m_Comms.Notify("DESIRED_ELEVATOR", my_elevator);
			m_Comms.Notify("MOOS_MANUAL_OVERIDE", "TRUE"); // see note in OnConnectCallBack
		} else {
			m_Comms.Notify("MOOS_MANUAL_OVERIDE", "FALSE");
		}

	}

	return 0;
}

void CMOOSRemote::save_message_log(void)
{
	string s;
	int c;

	getinput("Save file to:", s); if(s.size()) {
		FILE *fp;
		fp = fopen(s.c_str(), "w");
		for(c=0; (unsigned)c<info_messages.size(); c++) {
			fprintf(fp, "%s\n", info_messages[c].c_str());
		}
		fclose(fp);
		addim("Saved message log to `%s\'", s.c_str());
	} else {
		msgbox("Cancelled save!");
	}
}

bool CMOOSRemote::OnConnectCallBack(void *_arg)
{
	CMOOSRemote *remote = reinterpret_cast<CMOOSRemote *>(_arg);
	remote->m_Comms.Notify("MOOS_MANUAL_OVERIDE", "TRUE");
	// ^ overide is spelled wrong (it's override) just copied from iRemote
	return true;
}

bool CMOOSRemote::OnDisconnectCallBack(void *_arg)
{
	CMOOSRemote *remote = reinterpret_cast<CMOOSRemote *>(_arg);
	remote = remote;
	return true;
}

void CMOOSRemote::redraw_mainw(void)
{
	attr_on(A_BOLD, NULL);
        border(0,0,0,0,0,0,0,0);
        attr_off(A_BOLD, NULL);
        mvaddch(0, 2, '[' | A_BOLD);
        mvaddstr(0, 3, " MOOS remote control ");
        mvaddch(0, 3+strlen(" MOOS remote control "), ']' | A_BOLD);
        
        mvaddch(LINES-1, COLS-(4+strlen(" Press ? for help ")), '[');
        mvaddstr(LINES-1, COLS-(3+strlen(" Press ? for help ")), " Press ? for help ");
        mvaddch(LINES-1, COLS-3, ']');

	wnoutrefresh(stdscr);
}

void CMOOSRemote::redraw_statw(void)
{
	if(has_statw_changed)
		has_statw_changed = false;
	else
		return;
	
	wborder(statw,0,0,0,0,0,0,0,0);
	windtitle(statw, " Status ");
	
	mvwaddstr(statw, 1, 2, "  Thrust: ");
	mvwaddstr(statw, 2, 2, "  Rudder: ");
	mvwaddstr(statw, 3, 2, "Elevator: ");
	mvwaddstr(statw, 4, 2, " Control: ");

	wattr_on(statw, A_BOLD, NULL);
        wmove(statw,1,20);
        if(ABS(my_thrust) <= 20.0) { // OK
                wcolor_set(statw, COLOR_GREEN, NULL);
                waddstr(statw, "OK");
        } else if(ABS(my_thrust) > 20.0 && ABS(my_thrust) <= 60.0) {
                wcolor_set(statw, COLOR_YELLOW, NULL);
                waddstr(statw, "WN");
        } else {
                wcolor_set(statw, COLOR_RED, NULL);
                waddstr(statw, "!!");
        }
        
        wmove(statw, 2,20);
        if(ABS(my_rudder) <= 20.0) {
                wcolor_set(statw, COLOR_GREEN, NULL);
                waddstr(statw, "OK");
        } else if(ABS(my_rudder) > 20.0 && ABS(my_rudder) <= 60.0) {
                wcolor_set(statw, COLOR_YELLOW, NULL);
                waddstr(statw, "WN");
        } else {
                wcolor_set(statw, COLOR_RED, NULL);
                waddstr(statw, "!!");
        }
        wmove(statw, 3, 20);
        wcolor_set(statw, COLOR_YELLOW, NULL);
        waddstr(statw, "--");
        
        if(!is_auto) {
                wcolor_set(statw, COLOR_GREEN, NULL);
                mvwaddstr(statw, 4, 13, "  MANUAL ");
        } else {
                wcolor_set(statw, COLOR_YELLOW, NULL);
                mvwaddstr(statw, 4, 13, "AUTOMATIC");
        }
        
        wcolor_set(statw, COLOR_WHITE, NULL);
        wattr_off(statw, A_BOLD, NULL);
        
	char tmp[100];
	
        snprintf(tmp, 100, "%6.1f", my_thrust);
        mvwaddstr(statw, 1, 13, tmp);
        snprintf(tmp, 100, "%6.1f", my_rudder);
        mvwaddstr(statw, 2, 13, tmp);
        snprintf(tmp, 100, "%6.1f", my_elevator);
        mvwaddstr(statw, 3, 13, tmp);

	
	wnoutrefresh(statw);
}

void CMOOSRemote::redraw_posw(void)
{
        char tmp[100];
        
	if(has_posw_changed)
		has_posw_changed = false;
	else
		return;
	
        // robot
        wborder(posw,0,0,0,0,0,0,0,0);
        windtitle(posw, " Robot ");

        mvwaddstr(posw, 1, 2, "      X:");
        mvwaddstr(posw, 2, 2, "      Y:");
        mvwaddstr(posw, 3, 2, "HEADING:");
        mvwaddstr(posw, 4, 2, "   Type:");
        
        snprintf(tmp, 100, "%8.1f", robotx);
        mvwaddstr(posw, 1, 11, tmp);
        snprintf(tmp, 100, "%8.1f", roboty);
        mvwaddstr(posw, 2, 11, tmp);
        snprintf(tmp, 100, "%8.1f", robott);
        mvwaddstr(posw, 3, 11, tmp);
        snprintf(tmp, 100, "%8s", type.c_str()); // fix this
        mvwaddstr(posw, 4, 11, tmp);

        wnoutrefresh(posw);
}

void CMOOSRemote::redraw_logw(void)
{
	char tmp[100];

	if(has_logw_changed)
		has_logw_changed = false;
	else
		return;
        
        wborder(logw,0,0,0,0,0,0,0,0);
        windtitle(logw, " Logger ");

        mvwaddstr(logw, 1, 2, "Status: ");
        wattr_on(logw, A_BOLD, NULL);
        if(is_log_on) { // ok
                wcolor_set(logw, COLOR_GREEN, NULL);
                mvwaddstr(logw, 1, 10, "OK ");
        } else {
                wcolor_set(logw, COLOR_RED, NULL);
                mvwaddstr(logw, 1, 10, "ERR");
        }
        wattr_off(logw, A_BOLD, NULL);
        wcolor_set(logw, COLOR_WHITE, NULL);
        snprintf(tmp, 100, "%9s", is_log_on? loggerName.c_str() : "offline"); // fix
        mvwaddstr(logw, 1, 14, tmp);

        mvwaddstr(logw, 2, 2, "  Size: ");
        
        if(is_log_on) {
                struct stat info;
                stat(loggerFileName.c_str(), &info);
                if(info.st_size < 1024) {
                        snprintf(tmp, 100, "%li B", (long)info.st_size/1);
                } else if(info.st_size > 1024) {
                        snprintf(tmp, 100, "%li kB", (long)info.st_size/1024);
                } else if(info.st_size > 1048576) {
                        snprintf(tmp, 100, "%li MB", (long)info.st_size/1048576);
                }
                char tmp2[100];
                snprintf(tmp2, 100, "%-9s", tmp);
                strcpy(tmp, tmp2);
        } else {
                snprintf(tmp, 100, "---      ");
        }
        
        mvwaddstr(logw, 2, 10, tmp);
        
        mvwaddstr(logw, 3, 2, "Starts: ");
	if(is_log_on) {
        	struct tm ltm;
		localtime_r(&openTime, &ltm);
		strftime(tmp, 100, "%Y-%m-%d", &ltm);
		mvwaddstr(logw, 3, 10, tmp);
		strftime(tmp, 100, "%H:%M:%S", &ltm);
       		mvwaddstr(logw, 4, 10, tmp);
	} else {
		mvwaddstr(logw, 3, 10, "---       ");
		mvwaddstr(logw, 4, 10, "          ");
	}
        wnoutrefresh(logw);
}

#define SPACES "                                                                                                                                                                                                                                   "

void CMOOSRemote::redraw_infow(void)
{
	int i;
	
	if(has_infow_changed)
		has_infow_changed = false;
	else
		return;
	
	wborder(infow,0,0,0,0,0,0,0,0);
	windtitle(infow, " Messages ");

	for(i=0; i<(LINES-12) && (unsigned)i < info_messages.size() - infow_scroll; i++) {
		mvwaddnstr(infow, 1+i, 2, SPACES, COLS-10);
		mvwaddnstr(infow, 1+i, 2, info_messages[i+infow_scroll].c_str(), COLS-10);
	}

	for(; i<(LINES-12); i++) {
		mvwaddnstr(infow, 1+i, 2, SPACES, COLS-10);
	}

	wnoutrefresh(infow);
}

void CMOOSRemote::redraw_all(void)
{
	redraw_mainw();
	redraw_statw();
	redraw_posw();
	redraw_logw();
	redraw_infow();
	
	doupdate();
	move(LINES-1, COLS-1);
}

bool CMOOSRemote::comms_main(void *_arg)
{
	CMOOSRemote *p = reinterpret_cast<CMOOSRemote *>(_arg);
	
	p->m_Comms.Register("LOGGER_NAME", 0);
	p->m_Comms.Register("LOGGER_FILENAME", 0);
	p->m_Comms.Register("LOGGER_START", 0);
	
	p->m_Comms.Register("ROBOT_X", 0);
	p->m_Comms.Register("ROBOT_Y", 0);
	p->m_Comms.Register("ROBOT_HEADING", 0);

	p->m_Comms.Register("DESIRED_THRUST", 0);
	p->m_Comms.Register("DESIRED_RUDDER", 0);
	p->m_Comms.Register("DESIRED_ELEVATOR", 0);
	
	p->m_Comms.Register("MOOS_DEBUG", 0);
	
	// WARNING: this function is not thread safe, should be fixed!
	
	while(1) {
		MOOSMSG_LIST inmail;
		MOOSMSG_LIST::iterator it;

		p->m_Comms.Fetch(inmail);
		
		for(it = inmail.begin(); it != inmail.end(); it++) {
			if(it->IsSkewed(MOOSTime())) continue;
			
			if(it->m_sKey == "LOGGER_FILENAME") {
				p->loggerFileName = it->m_sVal;
				p->has_logw_changed = true;
			} else if(it->m_sKey == "LOGGER_START") {
				p->openTime = atoi(it->m_sVal.c_str());
				p->has_logw_changed = true;
			} else if(it->m_sKey == "LOGGER_NAME") {
				p->loggerName = it->m_sVal;
				p->logLastHit = (long)MOOSTime();
				p->is_log_on = true;
				p->has_logw_changed = true;
			} else if(it->m_sKey == "ROBOT_X") {
				p->robotx = it->m_dfVal;
				p->has_posw_changed = true;
			} else if(it->m_sKey == "ROBOT_Y") {
				p->roboty = it->m_dfVal;
				p->has_posw_changed = true;
			} else if(it->m_sKey == "ROBOT_HEADING") {
				p->robott = it->m_dfVal;
				p->has_posw_changed = true;
			} else if(it->m_sKey == "MOOS_DEBUG") {
				p->addim("[%s]: %s", it->m_sSrc.c_str(), it->m_sVal.c_str());
				p->has_infow_changed = true;
			}

			if(p->is_auto) {
				if(it->m_sKey == "DESIRED_THRUST") {
					p->my_thrust = it->m_dfVal;
					p->has_statw_changed = true;
				} else if(it->m_sKey == "DESIRED_RUDDER") {
					p->my_rudder = it->m_dfVal;
					p->has_statw_changed = true;
				} else if(it->m_sKey == "DESIRED_ELEVATOR") {
					p->my_elevator = it->m_dfVal;
					p->has_statw_changed = true;
				}
			}
		}

		inmail.clear();

		if(p->is_log_on && p->logLastHit + 2 < long(MOOSTime())) {
			p->is_log_on = false;
			p->has_logw_changed = true;
		}

		usleep(1000000/40);
	}

        return true;
}

void CMOOSRemote::Run()
{
	addim("Running MOOS Comm Client");
	m_Comms.Run(mServer.c_str(), mPort, "MOOSRemote", 40);
	sleep(2);
	
	addim("Starting up");
	setup_curses();
	create_windows();
	infow_scroll = 0;
	redraw_all();
	
        comms_thread.Initialise(comms_main, this);
        comms_thread.Start();
	
	addim("MOOSRemote ready");
	main_loop();

	endwin();
}
