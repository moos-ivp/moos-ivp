/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MY_Hold_Browser.h                                    */
/*    DATE: May 13th 2005                                        */
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

#ifndef MY_HOLD_BROWSER_HEADER
#define MY_HOLD_BROWSER_HEADER

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Hold_Browser.H>

// This version of the Fl_Hold_Browser simply ignores the up and
// down arrow keyboard events if the mouse is not over the region
// where the browser is rendered.

class MY_Hold_Browser : public Fl_Hold_Browser {
public:
  MY_Hold_Browser(int x, int y, int w, int h, const char *l=0) :
    Fl_Hold_Browser(x, y, w, h, l) {hx=x;hy=y;hw=w;hh=h;};

  int  handle(int event) {

    if(event == FL_KEYBOARD) {
      if((Fl::event_key()==FL_Up)   || (Fl::event_key()==FL_Down)) {
	int vx = Fl::event_x();
	int vy = Fl::event_y();
	if((vx < hx) || (vx > (hx+hw)) || (vy < hy) || (vy > (hy+hh)))
	  return(0);
      }
    }
    return(Fl_Hold_Browser::handle(event));
  }

 protected:
  int hx, hy, hw, hh;

};
#endif








